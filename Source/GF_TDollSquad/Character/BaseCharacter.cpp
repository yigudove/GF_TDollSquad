#include "BaseCharacter.h"

#include  "OnlineSessionSettings.h"
#include "GF_TDollSquad/Item/Weapon/BaseWeapon.h"

#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "GF_TDollSquad/HUD/OverheadWidget.h"

#include "OnlineSubsystem.h"
#include "CharacterComponent/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CharacterCameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetMesh());
	CameraSpringArm->TargetArmLength = 300.0f;
	CameraSpringArm->bUsePawnControlRotation = true;

	CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterFollowCamera"));
	CharacterCamera->SetupAttachment(CameraSpringArm);
	CharacterCamera->bUsePawnControlRotation = false;
	CharacterCamera->bCameraMeshHiddenInGame = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadInfo = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadInfo->SetupAttachment(GetMesh());
	OverheadInfo->SetupAttachment(RootComponent);

	CharacterCombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CharacterCombatComponent->SetIsReplicated(true);
	CharacterCombatComponent->Character = this;

	// ABaseWeapon *SpawnWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultItemBP);
	// CharacterCombatComponent->EquipWeapon(SpawnWeapon);

	LastTraceItem = nullptr;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			FString CharacterName = GetName();
			FVector CameraLocation = CameraManager->GetCameraLocation();
			// DrawDebugSphere(GetWorld(), CameraLocation, 24.0f, 12, FColor::Red);
		}
	}
	
	// DrawDebugSphere(GetWorld(), CharacterCamera->bCameraMeshHiddenInGame = false, 12.0f, 12, FColor::Purple);
	//
	// for(ABaseItem *Item : OverlappingItems)
	// {
	// 	Item->SetDropInfoWidgetVisibility(true);
	// }
	// GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Overlap Num: %d"), OverlappingItems.Num()));
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ABaseCharacter, OverlappingItems, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME(ABaseCharacter, RemovedOverlappingItem);
	DOREPLIFETIME(ABaseCharacter, LastTraceItem);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PEI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		PEI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		PEI->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		PEI->BindAction(InteractAction, ETriggerEvent::Started, this, &ABaseCharacter::Interact);
		PEI->BindAction(QuitGameAction, ETriggerEvent::Started, this, &ABaseCharacter::QuitGame);
		
		PEI->BindAction(FireAction, ETriggerEvent::Started, this, &ABaseCharacter::FirePressed);
		PEI->BindAction(FireAction, ETriggerEvent::Ongoing, this, &ABaseCharacter::FireOngoing);
		PEI->BindAction(FireAction, ETriggerEvent::Completed, this, &ABaseCharacter::FireReleased);

		PEI->BindAction(SpFireAction, ETriggerEvent::Started, this, &ABaseCharacter::SpFirePressed);
		PEI->BindAction(SpFireAction, ETriggerEvent::Ongoing, this, &ABaseCharacter::SpFireOngoing);
		PEI->BindAction(SpFireAction, ETriggerEvent::Completed, this, &ABaseCharacter::SpFireReleased);
	}
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(CharacterCombatComponent)
	{
		CharacterCombatComponent->Character = this;
	}
}


void ABaseCharacter::Move(const FInputActionValue& Value)
{
	// 转为2d向量
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 找出是哪个方向向前 获取控制旋转
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);//inPich和inRoll都为0但是yaw 也就是方向取控制旋转的方向
		//ue坐标系 前面 是x 垂直是z水平 是y
		// 获取向前向量
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    
		//获取向右向量
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);//围绕x轴 [ue-x代表向前的水平线]偏航叫 控制左右旋转 z代表高度垂直线
		AddControllerPitchInput(LookAxisVector.Y);//围绕y轴 [ue是水平线]俯仰角 roll  在unreal中，pitch 对应于y轴，yaw对应于z轴，roll对应于x轴。
	}
}

void ABaseCharacter::Interact(const FInputActionValue& Value)
{
	EquipWeapon(LastTraceItem);

}

void ABaseCharacter::FirePressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("FirePressed"));
	if(CharacterCombatComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("CharacterCombatComponent Exist"));
		CharacterCombatComponent->FireTrigger(true);
	}
}

void ABaseCharacter::FireOngoing(const FInputActionValue& Value)
{
}

void ABaseCharacter::FireReleased(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("FireReleased"));
	if(CharacterCombatComponent)
	{
		CharacterCombatComponent->FireTrigger(false);
	}
}

void ABaseCharacter::SpFirePressed(const FInputActionValue& Value)
{
	if(CharacterCombatComponent)
	{
		// CharacterCombatComponent->bAiming = true;
		CharacterCombatComponent->SetAiming(true);
	}
}

void ABaseCharacter::SpFireOngoing(const FInputActionValue& Value)
{
}

void ABaseCharacter::SpFireReleased(const FInputActionValue& Value)
{
	if(CharacterCombatComponent)
	{
		// CharacterCombatComponent->bAiming = false;
		CharacterCombatComponent->SetAiming(false);
	}
}

void ABaseCharacter::EquipWeapon(ABaseItem* WeaponToEquip)
{
	if(CharacterCombatComponent)
	{
		if(HasAuthority()) // on server
		{
			if(ABaseWeapon *DropWeapon = Cast<ABaseWeapon>(LastTraceItem))
			{
				CharacterCombatComponent->EquipItem(DropWeapon);
			}
		}
		else // on client
		{
			ServerEquipWeapon();
		}
	}
}

void ABaseCharacter::ServerEquipWeapon_Implementation()
{
	if(CharacterCombatComponent)
	{
		if(ABaseWeapon *DropWeapon = Cast<ABaseWeapon>(LastTraceItem))
	    {
	            CharacterCombatComponent->EquipItem(DropWeapon);
	    }
	}
}

void ABaseCharacter::QuitGame(const FInputActionValue& Value)
{
	UKismetSystemLibrary::QuitGame(GetWorld(),  GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}



void ABaseCharacter::SetLastTraceItem(ABaseItem* TraceItem)
{
	LastTraceItem = TraceItem;
}

void ABaseCharacter::OnRep_ClientOverlappingItem()
{
	for(ABaseItem *Item : OverlappingItems)
	{
		Item->SetDropInfoWidgetVisibility(true);
	}
}

void ABaseCharacter::OnRep_ClientRemoveOverlappingItem(ABaseItem* RemovedItem)
{
	// FString str = RemovedOverlappingItem == nullptr ? FString("NULL ITEM") : RemovedOverlappingItem->GetName();
	// UE_LOG(LogTemp, Log, TEXT("OnRep_ClientRemoveOverlappingItem Trigger, Item Num: %d, RemovedOverlappingItem: %s"), OverlappingItems.Num(), *str);
	if(RemovedOverlappingItem)
	{
		RemovedOverlappingItem->SetDropInfoWidgetVisibility(false);
	}
}

void ABaseCharacter::SetRemovedOverlappingItem(ABaseItem* Item)
{
	RemovedOverlappingItem = Item;
}


void ABaseCharacter::AddOverlappingItem(ABaseItem* OverlappingItem)
{
	OverlappingItems.Emplace(OverlappingItem);
	if(IsLocallyControlled())
	{
		if(OverlappingItem)
		{
			OverlappingItem->SetDropInfoWidgetVisibility(true);
		}
	}
}

void ABaseCharacter::RemoveOverlappingItem(ABaseItem* OverlappingItem)
{
	OverlappingItems.Remove(OverlappingItem);
	if(IsLocallyControlled())
	{
		if(OverlappingItem)
		{
			OverlappingItem->SetDropInfoWidgetVisibility(false);
		}
	}
}

void ABaseCharacter::PlayFireMontage(bool bAiming)
{
	if(CharacterCombatComponent == nullptr || CharacterCombatComponent->EquippedItem == nullptr) return;

	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		UE_LOG(LogTemp, Log, TEXT("AnimInstance && FireWeaponMontage"));
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		// SectionName = bAiming ? FName("AimingFire") : FName("HipFire");
		SectionName = FName("HipFire");
		AnimInstance->Montage_JumpToSection(SectionName);
		
	}
}

bool ABaseCharacter::IsAiming()
{
	return (CharacterCombatComponent && CharacterCombatComponent->bAiming);
}

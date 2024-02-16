#include "BaseCharacter.h"

#include  "OnlineSessionSettings.h"
#include "GF_TDollSquad/Item/BaseWeapon.h"

#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "GF_TDollSquad/HUD/OverheadWidget.h"

#include "OnlineSubsystem.h"
#include "CharacterComponent/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GF_TDollSquad/Item/BaseWeapon.h"
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

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadInfo = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadInfo->SetupAttachment(GetMesh());
	OverheadInfo->SetupAttachment(RootComponent);

	CharacterCombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CharacterCombatComponent->SetIsReplicated(true);

	// ABaseWeapon *SpawnWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultItemBP);
	// CharacterCombatComponent->EquipWeapon(SpawnWeapon);
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

	// DOREPLIFETIME(ABaseCharacter, LastOverlapItem);
	// DOREPLIFETIME_CONDITION(ABaseCharacter, LastOverlapItem, COND_OwnerOnly, );
	// DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingItems, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION_NOTIFY(ABaseCharacter, OverlappingItems, COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME(ABaseCharacter, RemovedOverlappingItem);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PEI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		PEI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		PEI->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		PEI->BindAction(InteractAction, ETriggerEvent::Started, this, &ABaseCharacter::Interact);
		PEI->BindAction(QuitGameAction, ETriggerEvent::Started, this, &ABaseCharacter::QuitGame);
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
	UE_LOG(LogTemp, Log, TEXT("Trigger Interact Action."));

	if(CharacterCombatComponent && HasAuthority())
	{
		if(ABaseWeapon *DropWeapon = Cast<ABaseWeapon>(LastTraceItem))
		{
			CharacterCombatComponent->EquipWeapon(DropWeapon);
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

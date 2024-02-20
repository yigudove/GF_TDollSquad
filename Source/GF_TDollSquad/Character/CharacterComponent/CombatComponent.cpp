#include "CombatComponent.h"

#include "Components/SphereComponent.h"
#include "GF_TDollSquad/Character/BaseCharacter.h"
#include "GF_TDollSquad/Item/Weapon/BaseWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "GF_TDollSquad/HUD/GameplayHUD.h"

UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedItem);
	DOREPLIFETIME(UCombatComponent, TraceTarget);
}

void UCombatComponent::EquipItem(ABaseWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	EquippedItem = WeaponToEquip;
	EquippedItem->SetItemState(EItemState::EIS_Equipped);
	const USkeletalMeshSocket *HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedItem, Character->GetMesh());
	}
	EquippedItem->SetOwner(Character);
	EquippedItem->SetDropInfoWidgetVisibility(false);
	EquippedItem->GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UCombatComponent::SwapItem(ABaseWeapon* WeaponToSwap)
{
	DropItem();
	EquipItem(WeaponToSwap);
}

void UCombatComponent::DropItem()
{
	if (EquippedItem)
	{
		FDetachmentTransformRules DetachmentTransformRules(
			EDetachmentRule::KeepWorld, true);
		EquippedItem->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		// EquippedWeapon->SetItemState(EItemState::EItemState_Falling);
		// EquippedItem->ItemStartFalling();

		EquippedItem = nullptr;
	}
}




void UCombatComponent::FireTrigger(bool bTrigger)
{
	bFiring = bTrigger;

	if(bFiring)
	{
		ServerFireTigger(TraceTarget.ImpactPoint);
	}

}

void UCombatComponent::ServerFireTigger_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	MulticastFireTrigger(TraceHitTarget);
}

void UCombatComponent::MulticastFireTrigger_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	if(EquippedItem == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("MulticastFireTrigger_Implementation  EquippedItem == nullptr"));
		return;
	}

	if(Character)
	{
		UE_LOG(LogTemp, Log, TEXT("Component  FireTrigger"));
		Character->PlayFireMontage(bAiming);
		ABaseWeapon *EquipWeapon = Cast<ABaseWeapon>(EquippedItem);
		EquipWeapon->WeaponFire(TraceHitTarget);
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FHitResult HitResult;
	// if(Character->IsLocallyControlled() && Character->IsPlayerControlled())
	// {
	// 	CrosshairTrace(HitResult);
	// }
	CrosshairTrace(TraceTarget);
	// if(Character->IsLocallyControlled() && Character->IsPlayerControlled()) 
	// {
	// 	SetHUDCrosshair(DeltaTime);
	// }
	SetHUDCrosshair(DeltaTime);
}

void UCombatComponent::CrosshairTrace(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldPosition, CrosshairWorldDirection;
	float TraceScale = 10000.0f;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if(bScreenToWorld)
	{
		FVector Start; 
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
			if (CameraManager)
			{
				Start = CameraManager->GetCameraLocation();
				FRotator Direction = CameraManager->GetCameraRotation();
				CrosshairWorldDirection = CameraManager->GetCameraRotation().Vector();
			}
		}
		FVector End = Start + CrosshairWorldDirection * TraceScale;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End,ECC_Visibility);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.0f, 0, 2.0f);
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.0f, 12, FColor::Purple);
			if(ABaseItem *HitItem = Cast<ABaseItem>(TraceHitResult.GetActor()))
			{
				if(Character->GetOverlappingItems().Contains(HitItem))
				{
					Character->SetLastTraceItem(HitItem);
				}
			}
			else
			{
				Character->SetLastTraceItem(nullptr);
			}
		}
	}
}

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr) return;

	// CharacterController = CharacterController == nullptr ? Cast<ABasePlayerController>(Character->Controller) : CharacterController;
	CharacterController = CharacterController == nullptr ? Cast<APlayerController>(Character->Controller) : CharacterController;
	if(CharacterController)
	{
		GPHUD = GPHUD == nullptr ? Cast<AGameplayHUD>(CharacterController->GetHUD()) : GPHUD;
		if(GPHUD)
		{
			FVector2D SpeedRange(0.0f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D Mutiplier(0.0f, 1.0f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.0f;
					
			// GPHUD->SetCrosshairSpread(FMath::GetMappedRangeValueClamped(SpeedRange, Mutiplier, Velocity.Size())) ;
			// GPHUD->DrawHUD();
		}
	}
}
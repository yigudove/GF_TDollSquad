#include "CombatComponent.h"

#include "Components/SphereComponent.h"
#include "GF_TDollSquad/Character/BaseCharacter.h"
#include "GF_TDollSquad/Item/Weapon/BaseWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
		if(!TraceHitResult.bBlockingHit) { TraceHitResult.ImpactPoint = End; }
		else
		{
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.0f, 12, FColor::Purple);
			
			if(ABaseItem *HitItem = Cast<ABaseItem>(TraceHitResult.GetActor()))
			{
				Character->SetLastTraceItem(HitItem);
			}
			else
			{
				Character->SetLastTraceItem(nullptr);
			}
		}
	}
	
}

void UCombatComponent::FireTrigger(bool bTrigger)
{
	Firing = true;

	if(Firing)
	{
		ServerFireTigger();
	}

}

void UCombatComponent::MulticastFireTrigger_Implementation()
{
	if(EquippedItem == nullptr) return;
	if(Character)
	{
		UE_LOG(LogTemp, Log, TEXT("Component  FireTrigger"));
		Character->PlayFireMontage(false);
	}
}

void UCombatComponent::ServerFireTigger_Implementation()
{
	MulticastFireTrigger();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	// if(Character->IsLocallyControlled() && Character->IsPlayerControlled())
	// {
	// 	CrosshairTrace(HitResult);
	// }
	CrosshairTrace(HitResult);
}


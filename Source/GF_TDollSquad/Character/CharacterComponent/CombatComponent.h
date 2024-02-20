#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class ABaseItem;
class ABaseWeapon;
class AGameplayHUD;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GF_TDOLLSQUAD_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABaseCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

public:
	void EquipItem(ABaseWeapon *WeaponToEquip);
	void SwapItem(ABaseWeapon *WeaponToSwap);
	void DropItem();
	
	void CrosshairTrace(FHitResult &TraceHitResult);
	void SetHUDCrosshair(float DeltaTime);

	void FireTrigger(bool bTrigger);
	UFUNCTION(Server, Reliable)
	void ServerFireTigger(const FVector_NetQuantize TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireTrigger(const FVector_NetQuantize TraceHitTarget);

	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.0f;
	UPROPERTY(ReplicatedUsing = OnRep_Health,  VisibleAnywhere, Category = "Player Stats")
	float Health = 100.0f;

	UFUNCTION()
	void OnRep_Health();
private:
	UPROPERTY(VisibleAnywhere)
	ABaseCharacter* Character;
	UPROPERTY(VisibleAnywhere)
	APlayerController* CharacterController;
	UPROPERTY(VisibleAnywhere)
	AGameplayHUD *GPHUD;
	UPROPERTY(VisibleAnywhere, Replicated)
	ABaseItem *EquippedItem;
	UPROPERTY(VisibleAnywhere, Replicated)
	FHitResult TraceTarget;
	UPROPERTY(VisibleAnywhere)
	bool bFiring;
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bAiming;
	
};

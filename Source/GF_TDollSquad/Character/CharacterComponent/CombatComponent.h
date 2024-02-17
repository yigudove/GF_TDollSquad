#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class ABaseItem;
class ABaseWeapon;

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

	void FireTrigger(bool bTrigger);

	UFUNCTION(Server, Reliable)
	void ServerFireTigger();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireTrigger();
	
private:
	UPROPERTY(EditAnywhere)
	ABaseCharacter* Character;
	UPROPERTY(EditAnywhere, Replicated)
	ABaseItem *EquippedItem;

	bool Firing;
	
};

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

public:
	void EquipWeapon(ABaseWeapon *WeaponToEquip);

	void CrosshairTrace(FHitResult &TraceHitResult);
	
private:
	ABaseCharacter* Character;
	UPROPERTY(EditAnywhere)
	ABaseItem *EquippedItem;

};

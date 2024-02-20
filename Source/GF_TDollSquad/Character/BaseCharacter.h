#pragma once

#include "CoreMinimal.h"

#include "GF_TDollSquad/Item/BaseItem.h"

#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/WidgetComponent.h"

#include "BaseCharacter.generated.h"

class ABaseWeapon;

UCLASS()
class GF_TDOLLSQUAD_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent *CameraSpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent * CharacterCamera;

#pragma region InputSettings
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext *InputMapping;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction *MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction *LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction *InteractAction;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction *QuitGameAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction *FireAction;
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction *SpFireAction;
#pragma  endregion

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);

	void FirePressed(const FInputActionValue& Value);
	void FireOngoing(const FInputActionValue& Value);
	void FireReleased(const FInputActionValue& Value);

	void SpFirePressed(const FInputActionValue& Value);
	void SpFireOngoing(const FInputActionValue& Value);
	void SpFireReleased(const FInputActionValue& Value);

	void EquipWeapon(ABaseItem* WeaponToEquip);
	
	// client send equipweapon request to server
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon();
	
	void QuitGame(const FInputActionValue& Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	UWidgetComponent *OverheadInfo;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory", Replicated, ReplicatedUsing = OnRep_ClientOverlappingItem)
	TArray<ABaseItem*> OverlappingItems;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = OnRep_ClientRemoveOverlappingItem)
	ABaseItem *RemovedOverlappingItem;
public:
	UPROPERTY(EditInstanceOnly,Replicated, BlueprintReadWrite, Category = "Inventory")
	ABaseItem *LastTraceItem;

public:
	TArray<ABaseItem*> GetOverlappingItems() { return OverlappingItems; }
	void SetLastTraceItem(ABaseItem *TraceItem);
	
private:
	UFUNCTION()
	void OnRep_ClientOverlappingItem();
	UFUNCTION()
	void OnRep_ClientRemoveOverlappingItem(ABaseItem *RemovedItem);

public:
	void SetRemovedOverlappingItem(ABaseItem* Item);
	void AddOverlappingItem(ABaseItem *OverlappingItem);
	void RemoveOverlappingItem(ABaseItem *OverlappingItem);

private:
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent *CharacterCombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseItem> DefaultItemBP;

#pragma region Anim
	UPROPERTY(EditAnywhere, Category = Anim)
	class UAnimMontage *FireWeaponMontage;

public:
	void PlayFireMontage(bool bAiming);
	bool IsAiming();
#pragma endregion
};

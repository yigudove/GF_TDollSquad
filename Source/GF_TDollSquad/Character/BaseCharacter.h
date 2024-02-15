#pragma once

#include "CoreMinimal.h"

#include "GF_TDollSquad/Item/BaseItem.h"

#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/WidgetComponent.h"

#include "BaseCharacter.generated.h"

UCLASS()
class GF_TDOLLSQUAD_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

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
#pragma  endregion

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void QuitGame(const FInputActionValue& Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	UWidgetComponent *OverheadInfo;

private:
	UPROPERTY(VisibleAnywhere, Category = "Inventory", Replicated)
	TArray<ABaseItem*> OverlappingItems;

public:
	FORCEINLINE void AddOverlappingItem(ABaseItem *OverlappingItem) { OverlappingItems.Emplace(OverlappingItem); }
	FORCEINLINE void RemoveOverlappingItem(ABaseItem *OverlappingItem) { OverlappingItems.Remove(OverlappingItem); }
};

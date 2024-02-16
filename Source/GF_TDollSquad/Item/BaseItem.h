// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Null UMETA(DisplayName = "Null State"),
	EIS_Drop UMETA(DisplayName = "Drop State"),
	EIS_Picking UMETA(DisplayName = "Picking State"),
	EIS_Inventory UMETA(DisplayName = "Inventory State"),
	
	EIS_MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class GF_TDOLLSQUAD_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &Sweep
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:	
	// virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent *ItemMesh;
	// 用于检测重叠交互
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent *AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EItemState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent *ItemDropInfoWidget;
public:
	UFUNCTION()
	void SetDropInfoWidgetVisibility(bool bVisible);
	FORCEINLINE void SetItemState(EItemState State) { WeaponState = State; }
};

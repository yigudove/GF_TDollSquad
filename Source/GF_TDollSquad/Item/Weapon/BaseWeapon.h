// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseItem.h"
#include "BaseWeapon.generated.h"

class ABaseProjectile;

UCLASS()
class GF_TDOLLSQUAD_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()
public:
        ABaseWeapon();
        virtual void BeginPlay() override;


        
        UPROPERTY(VisibleAnywhere)
        class UProjectileComponent *WeaponProjectileComponent;
public:
        
        void WeaponFire(const FVector& TraceTarget);
};

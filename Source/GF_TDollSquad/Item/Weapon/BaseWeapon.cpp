// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "GF_TDollSquad/Item/Weapon/WeaponComponent/ProjectileComponent.h"

ABaseWeapon::ABaseWeapon()
{
        bReplicates = true;
        WeaponProjectileComponent = CreateDefaultSubobject<UProjectileComponent>(TEXT("CombatComponent"));
        WeaponProjectileComponent->SetIsReplicated(true);
        // WeaponProjectileComponent->Item = this;
}

void ABaseWeapon::BeginPlay()
{
        Super::BeginPlay();

        if(WeaponProjectileComponent)
        {
                WeaponProjectileComponent->Item = this;
        }
}

void ABaseWeapon::WeaponFire(const FVector& TraceTarget)
{
        if(!HasAuthority()) return;
        if(WeaponProjectileComponent)
        {
                WeaponProjectileComponent->SpawnProjectile(TraceTarget);
        }
}

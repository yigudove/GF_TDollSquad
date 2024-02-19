// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class GF_TDOLLSQUAD_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent *ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent *ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem *Tracer;

	class UParticleSystemComponent *TracerComponent;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

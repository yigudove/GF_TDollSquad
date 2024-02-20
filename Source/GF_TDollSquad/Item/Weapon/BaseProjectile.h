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
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent *HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent *ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent *ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem *Tracer;
	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent *TracerComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactParticle;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

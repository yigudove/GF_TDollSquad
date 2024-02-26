// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "GF_TDollSquad/Character/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectMovement"));
	ProjectileMovementComponent->UpdateComponentVelocity();
	ProjectileMovementComponent->bSimulationEnabled = true;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			ProjectileCollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if(HasAuthority())
	{
		ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	}
}

void ABaseProjectile::Destroyed()
{
	Super::Destroyed();

	if(ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(TracerComponent)
	{
		TracerComponent->DestroyComponent();
		TracerComponent = nullptr; // 可选，将指针置为空以防止意外使用
	}
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							FVector NormalImpulse, const FHitResult& Hit)
{
	ABaseCharacter *HitCharacter = Cast<ABaseCharacter>(OtherActor);

	if(HitCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("FFFFFFF"))
		HitCharacter->PlayHitReactMontage();
	}
	
	Destroy();
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GF_TDollSquad/Item/BaseItem.h"
#include "GF_TDollSquad/Item/Weapon/BaseProjectile.h"

// Sets default values for this component's properties
UProjectileComponent::UProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProjectileComponent::SpawnProjectile(const FVector& TraceTarget)
{
	UE_LOG(LogTemp, Log, TEXT("SpawnProjectile"));
	APawn * InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket * FirePointSocket = Item->GetItemMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(FirePointSocket)
	{
		FTransform SocketTransform =  FirePointSocket->GetSocketTransform(Item->GetItemMesh());
		FVector Transition = TraceTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = Transition.Rotation();
		if(ProjectileClass)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = Item->GetOwner();
			SpawnParameters.Instigator = InstigatorPawn;
			UWorld *World = GetWorld();
			if(World)
			{
				World->SpawnActor<ABaseProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParameters
				);
			}
		}
	}
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "GF_TDollSquad/Character/BaseCharacter.h"


// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemAreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemDropInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemDropInfo"));
	ItemDropInfoWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	// if(GetLocalRole() == ROLE_Authority)
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereEndOverlap);
	}
	
	SetDropInfoWidgetVisibility(false);
}

void ABaseItem::SetDropInfoWidgetVisibility(bool bVisible)
{
	if(ItemDropInfoWidget)
	{
		ItemDropInfoWidget->SetVisibility(bVisible);
	}

}

void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	ABaseCharacter *BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if(BaseCharacter)
	{
		BaseCharacter->AddOverlappingItem(this);
	}
}

void ABaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter *BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if(BaseCharacter)
	{
		BaseCharacter->SetRemovedOverlappingItem(this);
		BaseCharacter->RemoveOverlappingItem(this);
	}
}




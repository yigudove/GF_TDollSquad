// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "GF_TDollSquad/Character/BaseCharacter.h"
#include "Net/UnrealNetwork.h"


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
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// ItemMesh->SetSimulatePhysics(false);
	// ItemMesh->SetEnableGravity(false);
	// ItemMesh->SetVisibility(true);
	// ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemAreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemDropInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemDropInfo"));
	ItemDropInfoWidget->SetupAttachment(RootComponent);
}

void ABaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseItem, ItemState);
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

void ABaseItem::SetItemState(EItemState State)
{

	// 	EIS_Null UMETA(DisplayName = "Null State"),
	// 	EIS_Drop UMETA(DisplayName = "Drop State"),
	// 	EIS_Picking UMETA(DisplayName = "Picking State"),
	// 	EIS_Inventory UMETA(DisplayName = "Inventory State"),
	// 	EIS_Equipped UMETA(DisplayName = "Equipped State"),
	//
	// 	EIS_MAX UMETA(DisplayName = "MAX")
	
	ItemState = State;
	switch (ItemState)
	{
	case EItemState::EIS_Equipped:
		SetDropInfoWidgetVisibility(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Drop:
		SetDropInfoWidgetVisibility(true);
		// AreaSphere->SetCollisionEnabled(ECollisionEnabled::);
		break;
	}
}

void ABaseItem::OnRep_SwitchItemState()
{
	switch (ItemState)
	{
	case EItemState::EIS_Equipped:
		SetDropInfoWidgetVisibility(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Drop:
		SetDropInfoWidgetVisibility(true);
		// AreaSphere->SetCollisionEnabled(ECollisionEnabled::);
		break;
	}
}

void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	if(ItemState == EItemState::EIS_Drop)
	{
		ABaseCharacter *BaseCharacter = Cast<ABaseCharacter>(OtherActor);
		if(BaseCharacter)
		{
			BaseCharacter->AddOverlappingItem(this);
		}
	}

}

void ABaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ItemState == EItemState::EIS_Drop)
	{
		ABaseCharacter *BaseCharacter = Cast<ABaseCharacter>(OtherActor);
        if(BaseCharacter)
        {
        	BaseCharacter->SetRemovedOverlappingItem(this);
        	BaseCharacter->RemoveOverlappingItem(this);
        }
	}

}




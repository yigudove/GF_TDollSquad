// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GF_TDOLLSQUAD_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual  void Logout(AController* Exiting) override;

	bool TravlToAnotherMap(FString MapPath = "/Game/GFContent/Maps/BasicMap");
};

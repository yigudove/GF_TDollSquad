// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GF_TDollSquadGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GF_TDOLLSQUAD_API AGF_TDollSquadGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
        virtual void PostLogin(APlayerController* NewPlayer) override;
        virtual  void Logout(AController* Exiting) override;
};

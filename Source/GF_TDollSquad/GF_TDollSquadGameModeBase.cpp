// Copyright Epic Games, Inc. All Rights Reserved.


#include "GF_TDollSquadGameModeBase.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AGF_TDollSquadGameModeBase::PostLogin(APlayerController* NewPlayer)
{
        Super::PostLogin(NewPlayer);

        if(GameState)
        {
                int32 PlayerNum = GameState.Get()->PlayerArray.Num();
                GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Num: %d"), PlayerNum));

                if(APlayerState *PlayerState = NewPlayer->GetPlayerState<APlayerState>())
                {
                        GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Login Player Name: %s"), *PlayerState->GetPlayerName()));
                }
        }
}

void AGF_TDollSquadGameModeBase::Logout(AController* Exiting)
{
        Super::Logout(Exiting);
        
        int32 PlayerNum = GameState.Get()->PlayerArray.Num();
        GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Num: %d"), PlayerNum));
        
        if(APlayerState *PlayerState = Exiting->GetPlayerState<APlayerState>())
        {
                GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Logout Player Name: %s"), *PlayerState->GetPlayerName()));
        }
}

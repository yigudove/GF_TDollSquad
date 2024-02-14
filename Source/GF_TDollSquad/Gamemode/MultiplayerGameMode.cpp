// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
        Super::PostLogin(NewPlayer);

        if(GameState)
        {
                int32 PlayerNum = GameState.Get()->PlayerArray.Num();
                GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Num: %d"), PlayerNum));

                if(APlayerState *PlayerState = NewPlayer->GetPlayerState<APlayerState>())
                {
                        // GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Login Player Name: %s"), *PlayerState->GetPlayerName()));
                        GEngine->AddOnScreenDebugMessage(
                                1,
                                15.0f,
                                FColor::Yellow,
                                FString::Printf(TEXT("Login Player Name: %s"), *PlayerState->GetPlayerName())
                        );
                }
        }
}

void AMultiplayerGameMode::Logout(AController* Exiting)
{
        Super::Logout(Exiting);
        
        int32 PlayerNum = GameState.Get()->PlayerArray.Num();
        GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Num: %d"), PlayerNum));
        
        if(APlayerState *PlayerState = Exiting->GetPlayerState<APlayerState>())
        {
                GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Logout Player Name: %s"), *PlayerState->GetPlayerName()));
        }
}

bool AMultiplayerGameMode::TravlToAnotherMap(FString MapPath)
{
        UWorld *World = GetWorld();
        if (World)
        {
                World->ServerTravel(MapPath + FString("?listen"));
                return true;
        }
        return false;
}

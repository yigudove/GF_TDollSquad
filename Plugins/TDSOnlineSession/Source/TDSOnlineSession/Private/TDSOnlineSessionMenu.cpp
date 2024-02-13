// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSOnlineSessionMenu.h"
#include "TDSSessionsSubsystem.h"
#include "Components/Button.h"


void UTDSOnlineSessionMenu::MenuSetup(int32 NumConnections, FString SessionName)
{
        NumPublicConnections = NumConnections;
        MatchTypeValue = SessionName;
        AddToViewport();
        SetVisibility(ESlateVisibility::Visible);
        bIsFocusable = true;

        UWorld *World = GetWorld();
        if(World)
        {
                APlayerController *PlayerController = World->GetFirstPlayerController();
                if(PlayerController)
                {
                        FInputModeUIOnly InputModeData;
                        InputModeData.SetWidgetToFocus(TakeWidget());
                        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                        PlayerController->SetInputMode(InputModeData);

                        PlayerController->SetShowMouseCursor(true);
                }
        }

        UGameInstance *GameInstance = GetGameInstance();
        if(GameInstance)
        {
               TDSSessionsSubsystem = GameInstance->GetSubsystem<UTDSSessionsSubsystem>();
        }

        if(TDSSessionsSubsystem)
        {
                TDSSessionsSubsystem->TDSOnCreateSessionComplete.AddDynamic(this, &ThisClass::UTDSOnlineSessionMenu::OnMenuCreateSession);
        }
}

bool UTDSOnlineSessionMenu::Initialize()
{
        if(! Super::Initialize())
        {
                return  false;
        }

        if(HostButton)
        {
                HostButton->OnClicked.AddDynamic(this, &UTDSOnlineSessionMenu::HostButtonClicked);
        }
        if(JoinButton)
        {
                JoinButton->OnClicked.AddDynamic(this, &UTDSOnlineSessionMenu::JoinButtonClicked);
        }

        return true;
}

void UTDSOnlineSessionMenu::NativeDestruct()
{
        MenuTearDown();
        Super::NativeDestruct();
}

void UTDSOnlineSessionMenu::OnMenuCreateSession(bool bSuccess)
{
        if(bSuccess)
        {
                GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Green,FString::Printf(TEXT("OnMenuCreateSession Success.")));

                UWorld *World = GetWorld();
                if(World)
                {
                        (World->ServerTravel(FString("/Game/GFContent/Maps/BasicMap?Listen")));
                }
        }
        else
        {
                GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Green,FString::Printf(TEXT("OnMenuCreateSession Failed.")));
        }
}

void UTDSOnlineSessionMenu::HostButtonClicked()
{
        if(TDSSessionsSubsystem)
        {
                TDSSessionsSubsystem->CreateSession(NumPublicConnections, MatchTypeValue);
        }
}

void UTDSOnlineSessionMenu::JoinButtonClicked()
{
        
}

void UTDSOnlineSessionMenu::MenuTearDown()
{
        RemoveFromParent();
        UWorld* World = GetWorld();
        if(World)
        {
                APlayerController *PC = World -> GetFirstPlayerController();
                if(PC)
                {
                        FInputModeGameOnly InputModeData;
                        PC -> SetInputMode(InputModeData);
                        PC -> SetShowMouseCursor(false);
                }
        }
}

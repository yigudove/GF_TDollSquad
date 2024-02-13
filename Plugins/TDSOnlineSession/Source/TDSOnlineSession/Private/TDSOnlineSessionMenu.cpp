// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSOnlineSessionMenu.h"
#include "TDSSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

#include "Components/Button.h"


void UTDSOnlineSessionMenu::MenuSetup(int32 NumConnections, FString SessionName,  FString MapPath)
{
        LevelPathTravelTo = MapPath + "?listen";
        NumPublicConnections = NumConnections;
        TDSSessionName = SessionName;
        AddToViewport();
        SetVisibility(ESlateVisibility::Visible);
        bIsFocusable = true;

        UWorld *World = GetWorld();
        if(World)
        {
                // 菜单输入模式
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
                // Bind Delegate
                TDSSessionsSubsystem->TDSOnCreateSessionComplete.AddDynamic(this, &ThisClass::UTDSOnlineSessionMenu::OnMenuCreateSession);
                TDSSessionsSubsystem->TDSOnFindSessionsComplete.AddUObject(this, &ThisClass::OnMenuFindSessions);
                TDSSessionsSubsystem->TDSOnJoinSessionComplete.AddUObject(this, &ThisClass::OnMenuJoinSession);
                TDSSessionsSubsystem->TDSOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestorySession);
                TDSSessionsSubsystem->TDSOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
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
        // 从菜单到关卡中，进行一些切换
        MenuTearDown();
        Super::NativeDestruct();
}

#pragma region SessionDelegate
void UTDSOnlineSessionMenu::OnMenuCreateSession(bool bSuccess)
{
        if(bSuccess)
        {
                GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Green,FString::Printf(TEXT("OnMenuCreateSession Success.")));

                UWorld *World = GetWorld();
                if(World)
                {
                        (World->ServerTravel(FString(LevelPathTravelTo)));
                }
        }
        else
        {
                GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Green,FString::Printf(TEXT("OnMenuCreateSession Failed.")));
                HostButton->SetIsEnabled(true);
        }
}

void UTDSOnlineSessionMenu::OnMenuFindSessions(const TArray<FOnlineSessionSearchResult>& SessionSearchResults,
        bool bSuccess)
{
        if(TDSSessionsSubsystem == nullptr)
        {
                return;
        }
        
        for( auto Result  : SessionSearchResults)
        {
                FString SettingsValue;
                Result.Session.SessionSettings.Get(FName("GFSession_Key"), SettingsValue);
                if(SettingsValue == TDSSessionName)
                {
                        TDSSessionsSubsystem -> JoinSession(Result);
                        return;
                }
        }

        if(!bSuccess || SessionSearchResults.Num() <= 0)
        {
                JoinButton->SetIsEnabled(true);
        }
}

void UTDSOnlineSessionMenu::OnMenuJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
        IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
        if(Subsystem)
        {
                IOnlineSessionPtr SessionInterface = Subsystem ->GetSessionInterface();
                if(SessionInterface.IsValid())
                {
                        FString Address;
                        SessionInterface -> GetResolvedConnectString(NAME_GameSession, Address);

                        if(APlayerController *PC = GetGameInstance()->GetFirstLocalPlayerController())
                        {
                                PC->ClientTravel(Address, TRAVEL_Absolute);
                        }
                }
        }

        if(Result != EOnJoinSessionCompleteResult::Success)
        {
                JoinButton->SetIsEnabled(true);
        }
}

void UTDSOnlineSessionMenu::OnDestorySession(bool bSuccess)
{
}

void UTDSOnlineSessionMenu::OnStartSession(bool bSuccess)
{
}
#pragma endregion

void UTDSOnlineSessionMenu::HostButtonClicked()
{
        HostButton->SetIsEnabled(false);
        if(TDSSessionsSubsystem)
        {
                TDSSessionsSubsystem->CreateSession(NumPublicConnections, TDSSessionName);
        }
}

void UTDSOnlineSessionMenu::JoinButtonClicked()
{
        JoinButton->SetIsEnabled(false);
        if(TDSSessionsSubsystem)
        {
                TDSSessionsSubsystem->FindSessions(10000);
        }
}

void UTDSOnlineSessionMenu::MenuTearDown()
{
        RemoveFromParent();
        UWorld* World = GetWorld();
        if(World)
        {
                // 非菜单输入模式
                APlayerController *PC = World -> GetFirstPlayerController();
                if(PC)
                {
                        FInputModeGameOnly InputModeData;
                        PC -> SetInputMode(InputModeData);
                        PC -> SetShowMouseCursor(false);
                }
        }
}

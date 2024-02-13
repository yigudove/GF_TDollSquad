// Fill out your copyright notice in the Description page of Project Settings.


 #include "TDSSessionsSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

UTDSSessionsSubsystem::UTDSSessionsSubsystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
 IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
 if(Subsystem)
 {
   SessionInterface = Subsystem->GetSessionInterface();
 }
}

#pragma region SessionFunc Call by Menu
bool UTDSSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchTypeValue)
{
        if(!SessionInterface.IsValid())
        {
                return false;
        }

        auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
        if(ExistingSession)
        {
                SessionInterface->DestroySession(NAME_GameSession);
        }

        CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

        LastSessionSetting = MakeShareable(new FOnlineSessionSettings());
        LastSessionSetting->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
        LastSessionSetting->NumPublicConnections = NumPublicConnections;
        LastSessionSetting->bAllowJoinInProgress  = true;
        LastSessionSetting -> bAllowJoinViaPresence = true;
        LastSessionSetting -> bShouldAdvertise= true;
        LastSessionSetting -> bUsesPresence = true;

        LastSessionSetting->Set(FName("GFSession_Key"), MatchTypeValue, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        if(!SessionInterface->CreateSession(*LocalPlayer -> GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSetting))
        {
                SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
                GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::Red,FString::Printf(TEXT("CreateSession Fail.")));
                TDSOnCreateSessionComplete.Broadcast(false);
        }
        else
        {
                GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Green,FString::Printf(TEXT("CreateSession Success.")));
        }

        return true;
}

void UTDSSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
       // FindSessionsCompleteDelegateHandle,



}

void UTDSSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionSearchResult)
{
        //JoinSessionCompleteDelegateHandle,
}

void UTDSSessionsSubsystem::DestroySession()
{
        //DestroySessionCompleteDelegateHandle,
}

void UTDSSessionsSubsystem::StartSession()
{
        //StartSessionCompleteDelegateHandle;
}
#pragma endregion

#pragma region Session Callback Func
void UTDSSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
        if(SessionInterface)
        {
                SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        }

        TDSOnCreateSessionComplete.Broadcast(bSuccess);
}

void UTDSSessionsSubsystem::OnFindSessionsComplete(bool bSuccess)
{
}

void UTDSSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UTDSSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
}

void UTDSSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bSuccess)
{
}
#pragma endregion

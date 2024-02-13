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
        
        if(auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession))
        {
                bCreateSessionOnDestroy = true;
                TempSessionInfo = SessionInfo(NumPublicConnections, MatchTypeValue);
                
                // SessionInterface->DestroySession(NAME_GameSession);
                DestroySession();
        }

        CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

        LastSessionSetting = MakeShareable(new FOnlineSessionSettings());
        LastSessionSetting->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
        LastSessionSetting->NumPublicConnections = NumPublicConnections;
        LastSessionSetting->bAllowJoinInProgress  = true;
        LastSessionSetting->bAllowJoinViaPresence = true;
        LastSessionSetting->bShouldAdvertise= true;
        LastSessionSetting->bUsesPresence = true;
        LastSessionSetting->bUseLobbiesIfAvailable = true;
        LastSessionSetting->BuildUniqueId = 1;

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
        if(!SessionInterface.IsValid())
        {
                return;
        }
        FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

        LastSessionSearch = MakeShareable((new FOnlineSessionSearch()));
        LastSessionSearch -> MaxSearchResults = MaxSearchResults;
        LastSessionSearch -> bIsLanQuery = IOnlineSubsystem::Get() -> GetSubsystemName() == "NULL";
        LastSessionSearch -> QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

        const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        if(!SessionInterface -> FindSessions(*LocalPlayer -> GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
        {
                SessionInterface -> ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

                // Broadcast empty array and false
                TDSOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        }
}

void UTDSSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionSearchResult)
{
       if(!SessionInterface.IsValid())
       {
               TDSOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
               return;
       }
        JoinSessionCompleteDelegateHandle = SessionInterface -> AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

        const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        
        if(!SessionInterface -> JoinSession(*LocalPlayer -> GetPreferredUniqueNetId(), NAME_GameSession, SessionSearchResult))
        {
                SessionInterface -> ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
                TDSOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
        }
}

void UTDSSessionsSubsystem::DestroySession()
{
        if(!SessionInterface.IsValid())
        {
                TDSOnDestroySessionComplete.Broadcast(false);
                return;
        }
        
        DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
        if(! SessionInterface->DestroySession(NAME_GameSession))
        {
                SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
                TDSOnDestroySessionComplete.Broadcast(false);
        }
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
        if(SessionInterface)
        {
                SessionInterface -> ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
        }

        // if no valid session found
        if(LastSessionSearch->SearchResults.Num() <= 0)
        {
                TDSOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
                return;
        }

        TDSOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bSuccess);
}

void UTDSSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
        if(SessionInterface)
        {
                SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
        }
        TDSOnJoinSessionComplete.Broadcast(JoinResult);
}

void UTDSSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
        if(SessionInterface)
        {
                SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
        }

        if(bSuccess && bCreateSessionOnDestroy)
        {
                bCreateSessionOnDestroy = false;
                CreateSession(TempSessionInfo.NumPublicConnections, TempSessionInfo.MatchTypeValue);
        }
        TDSOnDestroySessionComplete.Broadcast(bSuccess);
}

void UTDSSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bSuccess)
{
}
#pragma endregion

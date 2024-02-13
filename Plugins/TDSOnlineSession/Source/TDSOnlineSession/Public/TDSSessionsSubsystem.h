// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"

#include "TDSSessionsSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDSOnCreateSessionComplete, bool, bSuccess);
DECLARE_MULTICAST_DELEGATE_TwoParams(FTDSOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionSearchResult, bool bSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FTDSOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDSOnDestroySessionComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDSOnStartSessionComplete, bool, bSuccess);

/**
 * 
 */
UCLASS()
class TDSONLINESESSION_API UTDSSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
        UTDSSessionsSubsystem();

#pragma region SessionFunc Call by Menu
        bool CreateSession(int32 NumPublicConnections, FString MatchTypeValue);
        void FindSessions(int32 MaxSearchResults);
        void JoinSession(const FOnlineSessionSearchResult &SessionSearchResult);

        void DestroySession();
        void StartSession();

        // Delegate for the menu to bind callback
        FTDSOnCreateSessionComplete TDSOnCreateSessionComplete;
        FTDSOnFindSessionsComplete TDSOnFindSessionsComplete;
        FTDSOnJoinSessionComplete TDSOnJoinSessionComplete;
        FTDSOnDestroySessionComplete TDSOnDestroySessionComplete;
        FTDSOnStartSessionComplete TDSOnStartSessionComplete;
#pragma endregion
        
protected:
#pragma region Session Callback Func
        void OnCreateSessionComplete(FName SessionName, bool bSuccess);
        void OnFindSessionsComplete(bool bSuccess);
        void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
        void OnDestroySessionComplete(FName SessionName, bool bSuccess);
        void OnStartSessionComplete(FName SessionName, bool bSuccess);
#pragma  endregion
private:
        IOnlineSessionPtr SessionInterface;
        TSharedPtr<FOnlineSessionSettings> LastSessionSetting;
        TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

#pragma region Callback Delegate
        FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
        FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
        FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
        FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
        FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;

        FDelegateHandle
        CreateSessionCompleteDelegateHandle,
        FindSessionsCompleteDelegateHandle,
        JoinSessionCompleteDelegateHandle,
        DestroySessionCompleteDelegateHandle,
        StartSessionCompleteDelegateHandle;
#pragma endregion
};

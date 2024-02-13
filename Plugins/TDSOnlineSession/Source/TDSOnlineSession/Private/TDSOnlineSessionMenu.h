// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TDSOnlineSessionMenu.generated.h"

/**
 * 
 */
UCLASS()
class UTDSOnlineSessionMenu : public UUserWidget
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumConnections = 8, FString SessionName = FString(TEXT("GFSession")), FString MapPath = FString(TEXT("/Game/GFContent/Maps/BasicMap")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	// Dynamic bind to TDSOnlineSession->TDSOnCreateSessionComplete
	// 动态绑定到创建Session委托，用于从菜单移动到关卡
	UFUNCTION()
	void OnMenuCreateSession(bool bSuccess);
	void OnMenuFindSessions(const TArray<FOnlineSessionSearchResult>& SessionSearchResults, bool bSuccess);
	void OnMenuJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestorySession(bool bSuccess);
	UFUNCTION()
	void OnStartSession(bool bSuccess);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UButton *HostButton;
	UFUNCTION()
	void HostButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UButton *JoinButton;
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	UPROPERTY()
	class UTDSSessionsSubsystem * TDSSessionsSubsystem;

	int32 NumPublicConnections {8};
	// Session MatchType Value as session name.
	// Session 键值对中的值，暂时视为房间名
	FString TDSSessionName {TEXT("GFSession")};
	FString LevelPathTravelTo{TEXT("/Game/GFContent/Maps/BasicMap?Listen")};
};

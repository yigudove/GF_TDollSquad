// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDSOnlineSessionMenu.generated.h"

/**
 * 
 */
UCLASS()
class UTDSOnlineSessionMenu : public UUserWidget
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumConnections = 8, FString SessionName = FString(TEXT("GFSession")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	// Call TDSSessionsSubsystem delegate
	UFUNCTION()
	void OnMenuCreateSession(bool bSuccess);
	
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
	// Session Name
	FString MatchTypeValue {TEXT("GFSession")};
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class GF_TDOLLSQUAD_API UOverheadWidget: public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ControllerName;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTextBlock* GetControllerNameBlock(){return ControllerName;}
	
	UFUNCTION(BlueprintCallable)
	void SetTextBlock(UTextBlock* BlockToSet, FString TextToSet);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn *CurPawn);

protected:
	virtual void NativeDestruct() override;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseCharacterStats.generated.h"

class UProgressBar;
class UTextBlock;
UCLASS()
class GF_TDOLLSQUAD_API UBaseCharacterStats : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar *HealthBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock *HealthText;
};

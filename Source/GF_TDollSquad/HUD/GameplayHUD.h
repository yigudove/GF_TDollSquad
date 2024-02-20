// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayHUD.generated.h"

class UTexture2D;

UCLASS()
class GF_TDOLLSQUAD_API AGameplayHUD : public AHUD
{
	GENERATED_BODY()

private:
        UPROPERTY(EditAnywhere)
        float CrosshairSpreadMax = 20.0f;
        UPROPERTY(VisibleAnywhere)
        float CrosshairSpread;
public:
        FORCEINLINE void SetCrosshairSpread(float Spread) { CrosshairSpread = Spread; }
        virtual void DrawHUD() override;
        void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D CrosshairSpreadVec);

        UPROPERTY(EditAnywhere, Category = Crosshair)
        UTexture2D *CrosshairCenter;
        UPROPERTY(EditAnywhere, Category = Crosshair)
        UTexture2D *CrosshairTop;
        UPROPERTY(EditAnywhere, Category = Crosshair)
        UTexture2D *CrosshairBottom;
        UPROPERTY(EditAnywhere, Category = Crosshair)
        UTexture2D *CrosshairLeft;
        UPROPERTY(EditAnywhere, Category = Crosshair)
        UTexture2D *CrosshairRight;
};

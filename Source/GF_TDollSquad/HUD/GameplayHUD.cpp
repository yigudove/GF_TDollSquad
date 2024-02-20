// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayHUD.h"

#include "BaseCharacterStats.h"
#include "Blueprint/UserWidget.h"

void AGameplayHUD::DrawHUD()
{
        Super::DrawHUD();

        FVector2D ViewportSize;
        if(GEngine)
        {
                GEngine->GameViewport->GetViewportSize((ViewportSize));
                const FVector2D ViewportCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
        
                if(CrosshairCenter)
                {
                        // Vector CenterSpread;
                        DrawCrosshair(CrosshairCenter, ViewportCenter, FVector2D(0.0f, 0.0f));
                }
                if(CrosshairTop)
                {
                        DrawCrosshair(CrosshairTop, ViewportCenter, FVector2D(CrosshairSpread, 0.0f));
                }
                if(CrosshairBottom)
                {
                        DrawCrosshair(CrosshairBottom, ViewportCenter, FVector2D(-CrosshairSpread, 0.0f));
                }
                if(CrosshairLeft)
                {
                        DrawCrosshair(CrosshairLeft, ViewportCenter, FVector2D(0.0f, -CrosshairSpread));
                }
                if(CrosshairRight)
                {
                        DrawCrosshair(CrosshairRight, ViewportCenter, FVector2D(0.0f, CrosshairSpread));
                }
        }
}

void AGameplayHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D CrosshairSpreadVec)
{
        const float TextureWidth = Texture->GetSizeX(), TextureHeight = Texture->GetSizeY();
        const FVector2D TextureDrawPoint(
                ViewportCenter.X - (TextureWidth / 2.0f) + CrosshairSpreadVec.X,
                ViewportCenter.Y - (TextureHeight / 2.0f) + CrosshairSpreadVec.Y
                );

        DrawTexture(
                Texture,
                TextureDrawPoint.X,
                TextureDrawPoint.Y,
                TextureWidth,
                TextureHeight,
                0.0f,
                0.0f,
                1.0f,
                1.0f,
                FLinearColor::White
        );
}

void AGameplayHUD::BeginPlay()
{
        Super::BeginPlay();
        
        AddCharacterStats();
}

void AGameplayHUD::AddCharacterStats()
{
        APlayerController *PlayerController = GetOwningPlayerController();
        if(PlayerController && CharacterStatsClass)
        {
                CharacterStats = CreateWidget<UBaseCharacterStats>(PlayerController, CharacterStatsClass);
                CharacterStats->AddToViewport();
        }
}

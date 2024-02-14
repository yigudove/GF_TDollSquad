// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"

void UOverheadWidget::SetTextBlock(UTextBlock* BlockToSet, FString TextToSet)
{
        if(BlockToSet)
        {
                BlockToSet->SetText(FText::FromString(TextToSet));
        }
}

void UOverheadWidget::ShowPlayerNetRole(APawn* CurPawn)
{
        FString LocalNetRole;
        switch(ENetRole LocalRole = CurPawn->GetLocalRole())
        {
                case ENetRole::ROLE_Authority:
                        LocalNetRole = FString("Authority");
                        break;
                case ENetRole::ROLE_AutonomousProxy:
                        LocalNetRole = FString("AutonomousProxy");
                        break;
                case ENetRole::ROLE_SimulatedProxy:
                        LocalNetRole = FString("SimulatedProxy");
                        break;
                case ENetRole::ROLE_None:
                        LocalNetRole = FString("None (This is abnormal, report to developer.)");
                        break;
                case ENetRole::ROLE_MAX:
                        LocalNetRole = FString("Max (This is abnormal, report to developer.)");
                        break;
        }

        FString RemotelNetRole;
        switch(ENetRole RemoteRole = CurPawn->GetRemoteRole())
        {
                case ENetRole::ROLE_Authority:
                        RemotelNetRole = FString("Authority");
                        break;
                case ENetRole::ROLE_AutonomousProxy:
                        RemotelNetRole = FString("AutonomousProxy");
                        break;
                case ENetRole::ROLE_SimulatedProxy:
                        RemotelNetRole = FString("SimulatedProxy");
                        break;
                case ENetRole::ROLE_None:
                        RemotelNetRole = FString("None (This is abnormal, report to developer.)");
                        break;
                case ENetRole::ROLE_MAX:
                        RemotelNetRole = FString("Max (This is abnormal, report to developer.)");
                        break;
        }

        SetTextBlock(ControllerName, FString("LocalNetRole: ") + LocalNetRole + FString(" RemoteNetRole: ") + RemotelNetRole);
}

void UOverheadWidget::NativeDestruct()
{
        RemoveFromParent();
        Super::NativeDestruct();
}

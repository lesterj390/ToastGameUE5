// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TBC_HUD.h"

ATBC_HUD::ATBC_HUD()
{

}

void ATBC_HUD::DrawHUD()
{
	Super::DrawHUD();
}


void ATBC_HUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerStatsWidgetClass) {
		PlayerStatsWidget = CreateWidget<UPlayerStatsWidget>(GetWorld(), PlayerStatsWidgetClass);

		if (PlayerStatsWidget) {
			PlayerStatsWidget->AddToViewport();
		}
	}

}

void ATBC_HUD::UpdateStaminaPercent(float CurrentStamina)
{
	if (PlayerStatsWidget) {
		PlayerStatsWidget->UpdateStaminaPercent(CurrentStamina);
	}
}



void ATBC_HUD::UpdateSanityPercent(float CurrentSanity)
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->UpdateSanityPercent(CurrentSanity);
	}
}
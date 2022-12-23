// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Components/WidgetComponent.h"
#include "UI/PlayerStatsWidget.h"

#include "TBC_HUD.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API ATBC_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ATBC_HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void UpdateStaminaPercent(float CurrentStamina);

	void UpdateSanityPercent(float CurrentSanity);

public:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> PlayerStatsWidgetClass;

	UPlayerStatsWidget* PlayerStatsWidget;

private:
	/** Crosshair asset pointer */

};


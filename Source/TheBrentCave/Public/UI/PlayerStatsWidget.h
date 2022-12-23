// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"

#include "PlayerStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UPlayerStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UProgressBar* StaminaBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UImage* SanityMeter;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UImage* SelectedItem;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UVerticalBox* BatteryBox;


public:
	UPlayerStatsWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateStaminaPercent(float CurrentStamina);

	void UpdateSanityPercent(float CurrentSanity);


protected:
	float StaminaPercent;

	float SanityPercent;


protected:


};

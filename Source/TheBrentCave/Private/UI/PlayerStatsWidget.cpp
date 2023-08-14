// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerStatsWidget.h"
#include "Components/Border.h"

UPlayerStatsWidget::UPlayerStatsWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayerStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerStatsWidget::UpdateStaminaPercent(float CurrentStamina)
{
	StaminaPercent = CurrentStamina / 100;
	StaminaBar->SetPercent(StaminaPercent);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), StaminaBar->Percent));
}

void UPlayerStatsWidget::UpdateSanityPercent(float CurrentSanity)
{
	SanityPercent = CurrentSanity / 100;
	SanityMeter->SetColorAndOpacity(FLinearColor(0.1, FMath::Lerp(0.01, 0.1, SanityPercent), FMath::Lerp(0.01, 0.1, SanityPercent), 1));
}

void UPlayerStatsWidget::ShowHintPrompt()
{
	PlayAnimation(PromptMoreInfoAnimation);
}

void UPlayerStatsWidget::HideHintPrompt()
{
	float animTime = GetAnimationCurrentTime(PromptMoreInfoAnimation);
	if (IsAnimationPlaying(PromptMoreInfoAnimation) && animTime <= 7.0f) {
		SetAnimationCurrentTime(PromptMoreInfoAnimation, 7.0f);
	}
}



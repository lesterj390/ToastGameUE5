// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/TextBlock.h"
#include "Engine/Font.h"
#include "Fonts/SlateFontInfo.h"
#include "HangmanWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UHangmanWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:

	UPROPERTY()
		FString sentence;
	
	void SetupWidget();

public:

	void GenerateWidget(FString SentenceP);
	void SetSentence(FString SentenceP);

	UPROPERTY()
		UFont* GameFont;

	UPROPERTY()
		FString playerSentence;

		TArray<char> AttemptedLetters;

protected:

	UPROPERTY()
		UCanvasPanel* Canvas;

	UPROPERTY()
		UBorder* Border;

	UPROPERTY()
		UTextBlock* sentenceTextBox;

private:
	virtual void NativeConstruct() override;

};

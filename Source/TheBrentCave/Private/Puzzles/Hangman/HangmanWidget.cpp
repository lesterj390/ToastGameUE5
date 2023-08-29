// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Hangman/HangmanWidget.h"

void UHangmanWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHangmanWidget::GenerateWidget(FString SentenceP)
{
	if (WidgetTree) {
		Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Canvas"));
		WidgetTree->RootWidget = Canvas;

		Canvas->SetVisibility(ESlateVisibility::Visible);

		Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), (FName(*FString::Printf(TEXT("Text Border")))));
		Canvas->AddChild(Border);
		Border->SetBrushColor(FLinearColor(0, 0, 0, 0));
		Border->SetHorizontalAlignment(HAlign_Center);
		Border->SetVerticalAlignment(VAlign_Center);

		sentenceTextBox = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), (FName(*FString::Printf(TEXT("Sentence")))));
		Border->AddChild(sentenceTextBox);
		sentenceTextBox->SetText(FText::FromString(playerSentence));

		UCanvasPanelSlot* borderSlot = (UCanvasPanelSlot*)Border->Slot;
		if (borderSlot) {
			borderSlot->SetSize(FVector2D(1200, 750));
			borderSlot->SetPosition(FVector2D(0, 0));
			borderSlot->SetZOrder(1);
		}

		UBorderSlot* sentenceSlot = (UBorderSlot*)sentenceTextBox->Slot;
		if (sentenceSlot) {
			FSlateFontInfo currentFont = sentenceTextBox->GetFont();
			currentFont.Size = 56;
			currentFont.OutlineSettings.OutlineSize = 4;
			currentFont.OutlineSettings.OutlineColor.Black;
			currentFont.LetterSpacing = 350;
			sentenceTextBox->SetFont(currentFont);
		}
	}

	RebuildWidget();

	SetSentence(SentenceP);
}

void UHangmanWidget::SetupWidget()
{
	if (WidgetTree) {
		sentenceTextBox->SetText(FText::FromString(playerSentence));

		RebuildWidget();
	}
}

void UHangmanWidget::SetSentence(FString SentenceP)
{
	sentence = SentenceP;
	playerSentence = "";

	for (int i = 0; i < sentence.Len(); i++) {
		char currentChar = sentence[i];

		if (currentChar == ' ') {
			playerSentence.Append(" ");
		}
		else if (currentChar == '.') {
			playerSentence.Append(".");
		}
		else if (AttemptedLetters.Contains(currentChar)) {
			playerSentence.AppendChar(currentChar);
		}
		else {
			playerSentence.Append("_");
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%s"), *playerSentence));
	SetupWidget();
}
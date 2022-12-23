// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Hangman/HangmanWidget.h"

void UHangmanWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHangmanWidget::SetSentence(FString SentenceP)
{

	sentence = SentenceP;

}
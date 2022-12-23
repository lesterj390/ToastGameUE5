// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	FString sentence;

public:

	void SetSentence(FString SentenceP);

protected:

private:
	virtual void NativeConstruct() override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
#include "HighlightedCellWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UHighlightedCellWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SpawnAnim;

	static const int LEFT = 0;
	static const int RIGHT = 1;
	static const int UP = 2;
	static const int DOWN = 3;

protected:
	UCanvasPanel* Canvas;

public:
	void SetDirectionAndScale(int Direction, float CellUnit);

protected:


private:
	virtual void NativeConstruct() override;
};
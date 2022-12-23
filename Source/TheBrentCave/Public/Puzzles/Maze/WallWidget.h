// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "WallWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UWallWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	static const int HORIZONTAL = 0;
	static const int VERTICAL = 1;

protected:
	UCanvasPanel* Canvas;

public:
	void SetRotationAndScale(int Rotation, float UnitScale);

protected:


private:
	virtual void NativeConstruct() override;
};

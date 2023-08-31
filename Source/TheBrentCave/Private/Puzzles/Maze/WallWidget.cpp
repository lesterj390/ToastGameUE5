// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Maze/WallWidget.h"

void UWallWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Canvas = Cast<UCanvasPanel>(GetRootWidget());
}

void UWallWidget::SetRotationAndScale(int Rotation, float UnitScale)
{
	if (!Canvas) {
		Canvas = Cast<UCanvasPanel>(GetRootWidget());
	}
	if (Canvas)
	{
		Canvas->SetRenderScale(FVector2D(UnitScale / 100, UnitScale / 100));
		if (Rotation == HORIZONTAL)
		{
			Canvas->SetRenderTransformAngle(0);
		}
		else if (Rotation == VERTICAL)
		{
			Canvas->SetRenderTransformAngle(90);
		}
	}
}


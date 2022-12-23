// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Maze/HighlightedCellWidget.h"


void UHighlightedCellWidget::NativeConstruct()
{
	Canvas = Cast<UCanvasPanel>(GetRootWidget());

	if (SpawnAnim) {
		PlayAnimation(SpawnAnim);
	}
}


void UHighlightedCellWidget::SetDirectionAndScale(int Direction, float UnitScale)
{
	if (Canvas)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Canvas Exists And Direction Set")));

		if (Direction == LEFT)
		{
			Canvas->SetRenderScale(FVector2D(-UnitScale / 100, UnitScale / 100));
			Canvas->SetRenderTranslation(FVector2D(UnitScale * 9/8, 0));
		}
		else if (Direction == RIGHT)
		{
			Canvas->SetRenderScale(FVector2D(UnitScale / 100, UnitScale / 100));
			Canvas->SetRenderTranslation(FVector2D(0, 0));
		}
		else if (Direction == UP)
		{
			Canvas->SetRenderScale(FVector2D(UnitScale / 100, UnitScale / 100));
			Canvas->SetRenderTransformAngle(-90);
			Canvas->SetRenderTranslation(FVector2D(UnitScale / 8, UnitScale * (7/4)));
		}
		else if (Direction == DOWN)
		{
			Canvas->SetRenderScale(FVector2D(UnitScale / 100, UnitScale / 100));
			Canvas->SetRenderTransformAngle(90);
			Canvas->SetRenderTranslation(FVector2D(UnitScale, -UnitScale / 8));
		}
	}
}

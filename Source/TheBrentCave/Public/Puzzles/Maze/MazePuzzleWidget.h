// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Maze/MazeAlgorithm.h"
#include "Maze/DataTypes.h"
#include "Puzzles/Maze/HighlightedCellWidget.h"
#include "Puzzles/Maze/WallWidget.h"
#include "MazePuzzleWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UMazePuzzleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	int Rows, Columns;

	int WidgetResolution;

	float CellSize;

	UPROPERTY()
		TSubclassOf<UHighlightedCellWidget> HighlightedCellClass;

	UPROPERTY()
		TSubclassOf<UWallWidget> WallClass;

	UPROPERTY()
		UHighlightedCellWidget* CurrentHighlightedCell;

protected:
	UPROPERTY()
		UButton* TestButton;

	UPROPERTY()
		TScriptInterface<IMazeAlgorithm> MazeAlgorithm;

	UPROPERTY(meta = (MustImplement = "MazeAlgorithm"))
		TSubclassOf<UObject> MazeAlgorithmClass;

	UPROPERTY()
		UCanvasPanel* Canvas;

	UPROPERTY()
		UCanvasPanel* CellsCanvas;

	UPROPERTY()
		UCanvasPanel* WallsCanvas;


public:
	UFUNCTION()
		void SetupWidget(int Rows, int Columns, int WidgetResolution);

	UFUNCTION()
		void SetMazeAlgorithm(TSubclassOf<UObject> MazeAlgorithmClassP);

	UFUNCTION()
		void SummonHighlightedCell(FCell CellLocation, int Direction);

	UFUNCTION()
		void SummonWall(FWall WallLocation, int Rotation);

	UFUNCTION()
		void ReverseHighlightedCell(FCell CellLocation);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
		void GenerateCellsAndWall();

private:
	virtual void NativeConstruct() override;
	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Maze/MazePuzzleWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"

//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("")));

void UMazePuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


void UMazePuzzleWidget::SetupWidget(int RowsP, int ColumnsP, int WidgetResolutionP) {
	if (WidgetTree)
	{
		Rows = RowsP;
		Columns = ColumnsP;
		MazeAlgorithm->SetDimensions(Rows, Columns);

		WidgetResolution = WidgetResolutionP;

		CellSize = (float)WidgetResolution / Rows;

		Canvas = Cast<UCanvasPanel>(GetRootWidget());
		WidgetTree->RootWidget = Canvas;

		GenerateCellsAndWall();
		MazeAlgorithm->GenerateRemovedWidgetWalls(WidgetTree);

		


	/*	UBorder* FoundCell = Cast<UBorder>(WidgetTree->FindWidget(FName(TEXT("(5, 6)"))));
		if (FoundCell) {
			FoundCell->SetBrushColor(FLinearColor(0, .3921, 1, 1));
		}

		UBorder* FoundWall = Cast<UBorder>(WidgetTree->FindWidget(FName(TEXT("(5, 5)|(5, 6)"))));
		if (FoundWall) {
			WidgetTree->RemoveWidget(FoundWall);
		}*/
	}

	RebuildWidget();
}

void UMazePuzzleWidget::SetMazeAlgorithm(TSubclassOf<UMazeAlgorithm> MazeAlgorithmClassP)
{
	MazeAlgorithmClass = MazeAlgorithmClassP;

	MazeAlgorithm = NewObject<UMazeAlgorithm>(this, MazeAlgorithmClass);
}


void UMazePuzzleWidget::SummonHighlightedCell(FCell CellLocation, int Direction)
{
	UHighlightedCellWidget* NewHighlightedCell = WidgetTree->ConstructWidget<UHighlightedCellWidget>(HighlightedCellClass, *FString::Printf(TEXT("Highlighted (%d, %d)"), CellLocation[0], CellLocation[1]));
	CellsCanvas->AddChild(NewHighlightedCell);
	NewHighlightedCell->SetDirectionAndScale(Direction, CellSize);
	((UCanvasPanelSlot*)NewHighlightedCell->Slot)->SetPosition(FVector2D(CellLocation[1] * CellSize + (CellSize / 8) - (CellSize / 16), CellLocation[0] * CellSize + (CellSize / 8) + (CellSize / 16)));
	
	CurrentHighlightedCell = NewHighlightedCell;
}


void UMazePuzzleWidget::SummonWall(FWall WallLocation, int Rotation)
{
	UWallWidget* NewWallWidget = WidgetTree->ConstructWidget<UWallWidget>(WallClass, *FString::Printf(TEXT("%s"), *WallLocation.ToString()));
	WallsCanvas->AddChild(NewWallWidget);
	NewWallWidget->SetRotationAndScale(Rotation, CellSize);

	if (Rotation == UWallWidget::HORIZONTAL)
	{
		((UCanvasPanelSlot*)NewWallWidget->Slot)->SetPosition(FVector2D(WallLocation.maxByRow()[1] * CellSize + (CellSize / 16), WallLocation.maxByRow()[0] * CellSize + (CellSize / 8)));
	}
	else if (Rotation == UWallWidget::VERTICAL)
	{
		((UCanvasPanelSlot*)NewWallWidget->Slot)->SetPosition(FVector2D(WallLocation.maxByCol()[1] * CellSize + (CellSize / 8), WallLocation.maxByCol()[0] * CellSize + (CellSize / 16)));
	}
}


void UMazePuzzleWidget::ReverseHighlightedCell(FCell CellLocation)
{
	UHighlightedCellWidget* HighlightedCell = Cast<UHighlightedCellWidget>(WidgetTree->FindWidget(*FString::Printf(TEXT("Highlighted %s"), *CellLocation.ToString())));

	if (HighlightedCell) {
		CurrentHighlightedCell = HighlightedCell;

		HighlightedCell->PlayAnimationReverse(HighlightedCell->SpawnAnim);
	}
}

TSharedRef<SWidget> UMazePuzzleWidget::RebuildWidget()
{
	auto Result = Super::RebuildWidget();

	if (WidgetTree)
	{
		//UPanelWidget* RootWidgetRef = WidgetTree->ConstructWidget<UPanelWidget>(UPanelWidget::StaticClass(), TEXT("RootWidget"));
		/*UPanelWidget* RootWidgetRef = Cast<UPanelWidget>(GetRootWidget());
		WidgetTree->RootWidget = RootWidgetRef;

		TestButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("TestButton"));
		RootWidgetRef->AddChild(TestButton);*/
	}

	return Result;
}


void UMazePuzzleWidget::GenerateCellsAndWall()
{
	// Creating Cells Panel

	CellsCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CellsCanvas"));
	Canvas->AddChild(CellsCanvas);

	((UCanvasPanelSlot*)CellsCanvas->Slot)->SetSize(FVector2D(1920, 1080));



	// Walls

	// Creating Walls Panel

	WallsCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("WallsPanel"));
	Canvas->AddChild(WallsCanvas);

	for (int row = 0; row < Rows; row++)
	{
		for (int col = 0; col < Columns; col++)
		{
			// HorizWall

			SummonWall(FWall(FCell(row - 1, col), FCell(row, col)), UWallWidget::HORIZONTAL);

			// VertWall

			SummonWall(FWall(FCell(row, col - 1), FCell(row, col)), UWallWidget::VERTICAL);
		}
	}

	// Last Horiz Walls
	for (int col = 0; col < Columns; col++)
	{
		SummonWall(FWall(FCell(Rows - 1, col), FCell(Rows, col)), UWallWidget::HORIZONTAL);
	}

	// Last Vert Walls
	for (int row = 0; row < Rows; row++)
	{
		SummonWall(FWall(FCell(row, Columns - 1), FCell(row, Columns)), UWallWidget::VERTICAL);
	}

	// Center Cell

	float SmallCellSize = CellSize * 7 / 8;

	UBorder* NewCell = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), *FString::Printf(TEXT("Highlighted (%d, %d)"), Rows / 2, Rows / 2));
	WallsCanvas->AddChild(NewCell);

	NewCell->SetBrushColor(FLinearColor(0, 0.145098, 0.368627, 1));

	((UCanvasPanelSlot*)NewCell->Slot)->SetSize(FVector2D(SmallCellSize, SmallCellSize));
	((UCanvasPanelSlot*)NewCell->Slot)->SetPosition(FVector2D(Rows / 2 * CellSize + (CellSize / 8) + (CellSize / 16), Rows / 2 * CellSize + (CellSize / 8) + (CellSize / 16)));

	// Exit wall
	FWall ExitWallLocation = FWall(FCell(-1, Columns / 2), FCell(0, Columns / 2));

	UWidget* ExitWall = WidgetTree->FindWidget(*ExitWallLocation.ToString());

	if (ExitWall)
	{
		WidgetTree->RemoveWidget(ExitWall);
	}


	/*UHighlightedCellWidget* NewWidget = WidgetTree->ConstructWidget<UHighlightedCellWidget>(HighlightedCellClass, *FString::Printf(TEXT("Burger (%d, %d)"), 6, 5));
	WallsCanvas->AddChild(NewWidget);
	NewWidget->SetDirectionAndScale(UHighlightedCellWidget::DOWN, CellSize);
	((UCanvasPanelSlot*)NewWidget->Slot)->SetPosition(FVector2D(5 * CellSize + (CellSize / 8) - (CellSize / 16), 6 * CellSize + (CellSize / 8) + (CellSize / 16)));

	NewWidget = WidgetTree->ConstructWidget<UHighlightedCellWidget>(HighlightedCellClass, *FString::Printf(TEXT("Burger (%d, %d)"), 4, 5));
	WallsCanvas->AddChild(NewWidget);
	NewWidget->SetDirectionAndScale(UHighlightedCellWidget::UP, CellSize);
	((UCanvasPanelSlot*)NewWidget->Slot)->SetPosition(FVector2D(5 * CellSize + (CellSize / 8) - (CellSize / 16), 4 * CellSize + (CellSize / 8) + (CellSize / 16)));

	NewWidget = WidgetTree->ConstructWidget<UHighlightedCellWidget>(HighlightedCellClass, *FString::Printf(TEXT("Burger (%d, %d)"), 5, 4));
	WallsCanvas->AddChild(NewWidget);
	NewWidget->SetDirectionAndScale(UHighlightedCellWidget::LEFT, CellSize);
	((UCanvasPanelSlot*)NewWidget->Slot)->SetPosition(FVector2D(4 * CellSize + (CellSize / 8) - (CellSize / 16), 5 * CellSize + (CellSize / 8) + (CellSize / 16)));

	NewWidget = WidgetTree->ConstructWidget<UHighlightedCellWidget>(HighlightedCellClass, *FString::Printf(TEXT("Burger (%d, %d)"), 5, 6));
	WallsCanvas->AddChild(NewWidget);
	NewWidget->SetDirectionAndScale(UHighlightedCellWidget::RIGHT, CellSize);
	((UCanvasPanelSlot*)NewWidget->Slot)->SetPosition(FVector2D(6 * CellSize + (CellSize / 8) - (CellSize / 16), 5 * CellSize + (CellSize / 8) + (CellSize / 16)));*/

}

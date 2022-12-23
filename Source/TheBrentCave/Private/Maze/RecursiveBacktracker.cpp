// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/RecursiveBacktracker.h"

URecursiveBacktracker::URecursiveBacktracker()
{
	
}


void URecursiveBacktracker::TestPrint()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Hello World From Recursive");
}


void URecursiveBacktracker::GenerateRemovedWalls(TSubclassOf<AActor> wallActor)
{
	firstCell = FCell(FMath::RandRange(0, numberOfRows-1), FMath::RandRange(0, numberOfColumns-1));

	visited.Add(firstCell);
	stack.Add(firstCell);

	while (stack.Num() > 0)
	{
		currentCell = stack.Last();
		//Removes last element from stack
		stack.RemoveAt(stack.Num() - 1);
		TArray<FCell> cellNeighbours = GetUnvisitedNeighbours(currentCell);

		if (cellNeighbours.Num() > 0) {
			stack.Add(currentCell);

			int nextCellIndex = FMath::RandRange(0, cellNeighbours.Num() - 1);
			nextCell = cellNeighbours[nextCellIndex];

			RemoveWall(FWall(currentCell, nextCell), wallActor);

			visited.Add(nextCell);
			stack.Add(nextCell);
		}
	}
}


void URecursiveBacktracker::GenerateRemovedWidgetWalls(UWidgetTree* WidgetTreeP)
{
	firstCell = FCell(FMath::RandRange(0, numberOfRows - 1), FMath::RandRange(0, numberOfColumns - 1));

	visited.Add(firstCell);
	stack.Add(firstCell);

	while (stack.Num() > 0)
	{
		currentCell = stack.Last();
		//Removes last element from stack
		stack.RemoveAt(stack.Num() - 1);
		TArray<FCell> cellNeighbours = GetUnvisitedNeighbours(currentCell);

		if (cellNeighbours.Num() > 0) {
			stack.Add(currentCell);

			int nextCellIndex = FMath::RandRange(0, cellNeighbours.Num() - 1);
			nextCell = cellNeighbours[nextCellIndex];

			//RemoveWall(FWall(currentCell, nextCell), wallActor);

			RemoveWidgetWall(FWall(currentCell, nextCell), WidgetTreeP);

			visited.Add(nextCell);
			stack.Add(nextCell);
		}
	}
}


void URecursiveBacktracker::BeginPlay()
{
	Super::BeginPlay();
}

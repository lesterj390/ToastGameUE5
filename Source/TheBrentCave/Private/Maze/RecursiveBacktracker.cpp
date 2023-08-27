// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/RecursiveBacktracker.h"

URecursiveBacktracker::URecursiveBacktracker()
{
	
}


void URecursiveBacktracker::GenerateRemovedWalls(TSubclassOf<AActor> wallActor)
{
	RemovedWalls.Empty();
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

			FWall toRemove = FWall(currentCell, nextCell);

			// Adding toRemove to the list of removed walls
			RemovedWalls.Add(toRemove);
			RemoveWall(toRemove, wallActor, GetWorld());

			visited.Add(nextCell);
			stack.Add(nextCell);
		}
	}
}


void URecursiveBacktracker::GenerateRemovedWalls(UWidgetTree* WidgetTree)
{
	RemovedWalls.Empty();
	firstCell = FCell(FMath::RandRange(0, numberOfRows - 1), FMath::RandRange(0, numberOfColumns - 1));

	visited.Add(firstCell);
	stack.Add(firstCell);

	while (stack.Num() > 0) {
		currentCell = stack.Last();
		//Removes last element from stack
		stack.RemoveAt(stack.Num() - 1);
		TArray<FCell> cellNeighbours = GetUnvisitedNeighbours(currentCell);

		if (cellNeighbours.Num() > 0) {
			stack.Add(currentCell);

			int nextCellIndex = FMath::RandRange(0, cellNeighbours.Num() - 1);
			nextCell = cellNeighbours[nextCellIndex];

			FWall toRemove = FWall(currentCell, nextCell);

			// Adding toRemove to the list of removed walls
			RemovedWalls.Add(toRemove);
			RemoveWall(toRemove, WidgetTree);

			visited.Add(nextCell);
			stack.Add(nextCell);
		}
	}
}
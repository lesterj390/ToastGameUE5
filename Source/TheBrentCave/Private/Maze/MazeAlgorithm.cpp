// Don't steal out game please :( we worked hard on it!


#include "Maze/MazeAlgorithm.h"

// Add default functionality here for any IMazeAlgorithm functions that are not pure virtual.

void IMazeAlgorithm::SetDimensions(int NumberOfRows, int NumberOfColumns)
{
	numberOfRows = NumberOfRows;
	numberOfColumns = NumberOfColumns;
}

TArray<FCell> IMazeAlgorithm::GetUnvisitedNeighbours(FCell cell)
{
	int row = cell[0];
	int column = cell[1];

	TArray<FCell> neighbours;

	//Up
	if (row > 0)
	{
		if (!visited.Contains(FCell(row - 1, column))) {
			neighbours.Add(FCell(row - 1, column));
		}
	}

	//Down
	if (row < numberOfRows - 1)
	{
		if (!visited.Contains(FCell(row + 1, column))) {
			neighbours.Add(FCell(row + 1, column));
		}
	}

	//Left
	if (column > 0)
	{
		if (!visited.Contains(FCell(row, column - 1))) {
			neighbours.Add(FCell(row, column - 1));
		}
	}

	//Right
	if (column < numberOfColumns - 1)
	{
		if (!visited.Contains(FCell(row, column + 1))) {
			neighbours.Add(FCell(row, column + 1));
		}
	}

	return neighbours;
}

void IMazeAlgorithm::RemoveWall(FWall wallToRemove, TSubclassOf<AActor> wallActor, const UObject* worldContextObject)
{
	TArray<AActor*> FoundActors;
	FString wallString = wallToRemove.ToString();
	UGameplayStatics::GetAllActorsOfClassWithTag(worldContextObject, wallActor, *wallString, FoundActors);

	if (FoundActors.Num() > 0) {
		FoundActors[0]->Destroy();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Maze Generation Error: Wall %s could not be found and removed."), *wallString);
	}
}

void IMazeAlgorithm::RemoveWall(FWall wallToRemove, UWidgetTree* WidgetTree)
{
	FString wallString = wallToRemove.ToString();
	UWidget* WallToRemove = WidgetTree->FindWidget(*wallString);
	if (WallToRemove) {
		WidgetTree->RemoveWidget(WallToRemove);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Maze Generation Error: Wall %s could not be found and removed."), *wallString);
	}
}
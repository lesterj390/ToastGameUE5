// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Maze/DataTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "MazeAlgorithm.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMazeAlgorithm : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEBRENTCAVE_API IMazeAlgorithm
{
	GENERATED_BODY()


	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void SetDimensions(int NumberOfRows, int NumberOfColumns);

	TArray<FCell> GetUnvisitedNeighbours(FCell cell);

	virtual void RemoveWall(FWall wallToRemove, TSubclassOf<AActor> wallActor, const UObject* worldContextObject);

	virtual void RemoveWall(FWall wallToRemove, UWidgetTree* WidgetTree);

	virtual void GenerateRemovedWalls(TSubclassOf<AActor> wallActor) = 0;

	virtual void GenerateRemovedWalls(UWidgetTree* WidgetTreeP) = 0;

protected:

public:
	TArray<FWall> RemovedWalls;

protected:
	TArray<FCell> visited;

	FCell currentCell;

	FCell firstCell;

	int numberOfRows;

	int numberOfColumns;
};

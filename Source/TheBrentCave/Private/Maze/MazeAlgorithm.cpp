// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeAlgorithm.h"

// Sets default values for this component's properties
UMazeAlgorithm::UMazeAlgorithm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	
}


void UMazeAlgorithm::TestPrint()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Hello World From Parent");
}


void UMazeAlgorithm::SetDimensions(int numberOfRowsP, int numberOfColumnsP)
{
	numberOfRows = numberOfRowsP;
	numberOfColumns = numberOfColumnsP;
}


TArray<FCell> UMazeAlgorithm::GetUnvisitedNeighbours(FCell cell)
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


void UMazeAlgorithm::RemoveWall(FWall wallToRemove, TSubclassOf<AActor> wallActor)
{
	TArray<AActor*> FoundActors;
	FString wallString = wallToRemove.ToString();
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), wallActor, *wallString, FoundActors);

	if (FoundActors.Num() > 0) {
		FoundActors[0]->Destroy();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s could not be found"), *wallString);
	}
}


void UMazeAlgorithm::RemoveWidgetWall(FWall wallToRemove, UWidgetTree* WidgetTreeP)
{
	UWidget* WallToRemove = WidgetTreeP->FindWidget(*(wallToRemove.ToString()));
	if (WallToRemove) {
		WidgetTreeP->RemoveWidget(WallToRemove);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "WallRemoved");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Coudln't find wall");
	}
}


void UMazeAlgorithm::GenerateRemovedWalls(TSubclassOf<AActor> wallActor)
{
	//Placeholder fuction so it can be seen in Grid
}


void UMazeAlgorithm::GenerateRemovedWidgetWalls(UWidgetTree* WidgetTreeP)
{
	//Placeholder fuction
}


// Called when the game starts
void UMazeAlgorithm::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
//void UMazeAlgorithm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}


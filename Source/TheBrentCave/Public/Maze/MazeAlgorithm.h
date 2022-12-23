// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Maze/DataTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "MazeAlgorithm.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEBRENTCAVE_API UMazeAlgorithm : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMazeAlgorithm();

	virtual void TestPrint();

	void SetDimensions(int numberOfRowsP, int numberOfColumnsP);

	TArray<FCell> GetUnvisitedNeighbours(FCell cell);

	void RemoveWall(FWall wallToRemove, TSubclassOf<AActor> wallActor);

	void RemoveWidgetWall(FWall wallToRemove, UWidgetTree* WidgetTreeP);

	virtual void GenerateRemovedWalls(TSubclassOf<AActor> wallActor);

	virtual void GenerateRemovedWidgetWalls(UWidgetTree* WidgetTreeP);

protected:
	TArray<FCell> visited;

	FCell currentCell;

	FCell firstCell;

	int numberOfRows;

	int numberOfColumns;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

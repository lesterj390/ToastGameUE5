// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maze/MazeAlgorithm.h"
#include "RecursiveBacktracker.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API URecursiveBacktracker : public UMazeAlgorithm
{
	GENERATED_BODY()

public:

	URecursiveBacktracker();

	virtual void TestPrint() override;

	virtual void GenerateRemovedWalls(TSubclassOf<AActor> wallActor) override;

	virtual void GenerateRemovedWidgetWalls(UWidgetTree* WidgetTreeP) override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	TArray<FCell> stack;

	FCell nextCell;
	
};

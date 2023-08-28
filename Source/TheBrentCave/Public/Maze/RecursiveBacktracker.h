// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maze/MazeAlgorithm.h"
#include "RecursiveBacktracker.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API URecursiveBacktracker : public UObject, public IMazeAlgorithm
{
	GENERATED_BODY()

public:
	URecursiveBacktracker();

	virtual void GenerateRemovedWalls(TSubclassOf<AActor> wallActor) override;

	virtual void GenerateRemovedWalls(UWidgetTree* WidgetTree) override;

protected:

protected:
	TArray<FCell> stack;

	FCell nextCell;	
};

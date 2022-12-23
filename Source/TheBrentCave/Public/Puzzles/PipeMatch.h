// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/DataTypes.h"
#include "PipeMatch.generated.h"

UCLASS()
class THEBRENTCAVE_API APipeMatch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APipeMatch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupPath();

	FCell nextPath(int row, int col);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PuzzleBase;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* NailMesh;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> Straight;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> Angle;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> StartPipe;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> EndPipe;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int PuzzleSize;

	float PieceSpacing;

	int PieceRotation;

	void GeneratePieces();

	int whichPiece;

	int lastRotation;
	int lastType;

protected:
	TArray<TArray<AActor*>> PipePieces;

	UPROPERTY()
		TArray<FCell> pipePath;

	UPROPERTY()
		TArray<FCell> visitedPath;

	float pipeLength;

};
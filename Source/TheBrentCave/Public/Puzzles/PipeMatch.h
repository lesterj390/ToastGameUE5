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

	void GeneratePieces();

	int whichPiece;

	int lastRotation;
	int lastType;

	enum direction { UP, DOWN, LEFT, RIGHT, NONE};

protected:
	bool inPuzzle;

	TArray<TArray<AActor*>> PipePieces;

	UPROPERTY()
		TArray<FCell> pipePath;

	UPROPERTY()
		TArray<FCell> visitedPath;

	float pipeLength;

	FVector startPipeLocation;

	FCell selectedPipe;
	UMaterialInterface* tempMat;
	UMaterialInstanceDynamic* PipeMat;
	
public:	
	// Sets default values for this actor's properties
	APipeMatch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupPath();

	FCell GetUnvisitedNeighbour(FCell cell);

	AActor* CreatePipe(int row, int col, TSubclassOf<AActor> pipeType, bool randomRotation);

	void SetupKeybinds();
	void Interact();

	void GetSelected();
	void GetInput();

	void SetupMaterials();

	int GetRotation(FCell cell);
	void RotateSelectedPipe();

	direction GetPipeInput(FCell pipe);
	direction GetPipeOutput(FCell pipe);
	TArray<direction> GetPipeDirections(FCell pipe);
	void DiscoverPathInputs();
	
	bool CheckForWin();
};
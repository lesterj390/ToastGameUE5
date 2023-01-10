// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "TBCCharacter.h"
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

	UPROPERTY(EditAnywhere)
		USpotLightComponent* Spotlight;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> breakPipeActor;

	UPROPERTY(EditAnywhere)
		USoundCue* RotateSound;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int MaxPuzzleSize;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int MinPuzzleSize;

	int PuzzleSize;

	float PieceSpacing;

	void GeneratePieces();

	int whichPiece;

	int lastRotation;
	int lastType;

	enum direction { UP, DOWN, LEFT, RIGHT, NONE};

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	bool inPuzzle;
	bool inOverlap;

	TArray<TArray<AActor*>> PipePieces;
	TArray<AActor*> CornerPipes;

	UPROPERTY()
		TArray<FCell> pipePath;

	UPROPERTY()
		TArray<FCell> visitedPath;

	float pipeLength;

	FVector startPipeLocation;

	FCell selectedPipe;
	UMaterialInterface* tempMat;
	UMaterialInstanceDynamic* PipeMat;

	FTimerHandle breakPuzzle;
	
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

	FString directionToString(direction d);

	bool PipeInBounds(FCell pipe);
	direction GetPipeInput(FCell pipe);
	direction GetPipeOutput(FCell pipe);
	TArray<direction> GetPipeDirections(FCell pipe);
	void DiscoverPathInputs();
	
	bool CheckForWin();

	void EnterPuzzle();
	void ExitPuzzle();

	void WinPuzzle();
};
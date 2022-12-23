// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/MazeAlgorithm.h"
#include "Maze/RecursiveBacktracker.h"
#include "Maze/DataTypes.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Field.h"
#include "GISetup.h"
#include "Math/UnrealMathUtility.h"
#include "Grid.generated.h"

UCLASS()
class THEBRENTCAVE_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();
	
	/*UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;*/


	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Rows;

	UPROPERTY(EditAnywhere)
		int32 Columns;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> CellActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> WallActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> CeilingActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> PillarActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TArray <TSubclassOf<AActor>> LockerActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> ToastActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		bool bSpawnToast;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		float LockerSpawningPercent;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> DoorKeyholeActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TArray <TSubclassOf<AActor>> PuzzleActorArray;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TArray <TSubclassOf<AActor>> PropActorArray;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		float PropSpawningPercent;

	UPROPERTY(EditAnywhere, Category = "CeilingPieces")
		TSubclassOf<AActor> LActor;

	UPROPERTY(EditAnywhere, Category = "CeilingPieces")
		TSubclassOf<AActor> IActor;

	UPROPERTY(EditAnywhere, Category = "CeilingPieces")
		TSubclassOf<AActor> TActor;

	UPROPERTY(EditAnywhere, Category = "CeilingPieces")
		TSubclassOf<AActor> XActor;

	UPROPERTY(EditAnywhere, Category = "CeilingPieces")
		TSubclassOf<AActor> UActor;

	// Maze Algorithm
	UPROPERTY(EditAnywhere, Category = "Maze Generation")
		TSubclassOf<UMazeAlgorithm> MazeAlgorithmSubclass;

	UPROPERTY(EditAnywhere, Category = "AI")
		TSubclassOf<AActor> NavMeshBoundsVolume;

	UPROPERTY()
		TArray <TSubclassOf<AActor>> puzzleArrayClone;

	//Toast Setup
	AVolume *ToastWalkableSurface;

	//Cosmetics
	//Backrooms//
	UPROPERTY(EditAnywhere, Category = "Cosmetic")
		TArray <TSubclassOf<AActor>> BackroomPropsArray;

	UPROPERTY(EditAnywhere, Category = "Cosmetic")
		TArray <TSubclassOf<AActor>> HotelPropsArray;

	// Players
	UPROPERTY(EditAnywhere, Category = "Players")
		int spawnedPlayers;

protected:
	UMazeAlgorithm* MazeAlgorithm;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
		void RemoveWalls();

	UFUNCTION()
		void SpawnActor(TSubclassOf<AActor> Actor, FVector Loc, FRotator Rot, FName CellTag, FName Path);
		void SpawnActor(TSubclassOf<AActor> Actor, FVector Loc, FRotator Rot, FVector Scale, FName CellTag, FName Path);

	UFUNCTION()
		void GenerateFloorAndCeiling();

	UFUNCTION()
		void GenerateWalls();

	UFUNCTION()
		void GenerateDoorKeyhole();


	UFUNCTION()
		TArray<FString> GetCellWalls(int Row, int Column);

	UFUNCTION()
		void GenerateLockers();

	UFUNCTION()
		void GeneratePuzzles();

	UFUNCTION()
		void GenerateProps();

	void SetupMazeAlgorithmComponent();

	void SetupToastWalkableSurface();

	void getSetupInfo();

	void beginGeneration();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

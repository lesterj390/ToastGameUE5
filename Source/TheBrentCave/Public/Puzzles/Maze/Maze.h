// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Puzzles/Maze/MazePuzzleWidget.h"
#include "Maze/MazeAlgorithm.h"
#include "Maze/DataTypes.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "TBCCharacter.h"
#include "Maze.generated.h"

UCLASS()
class THEBRENTCAVE_API AMaze : public AActor
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UMazePuzzleWidget> MazeWidgetClass;

	UPROPERTY(EditAnywhere, meta = (MustImplement = "MazeAlgorithm"))
		TSubclassOf<UObject> MazeAlgorithmClass;

	UPROPERTY(EditAnywhere, Category = "WidgetPieces")
		TSubclassOf<UUserWidget> HighlightedCellClass;

	UPROPERTY(EditAnywhere, Category = "WidgetPieces")
		TSubclassOf<UUserWidget> WallClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
		int Dimensions;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PuzzleMesh;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* MazeWidgetComponent;

	UPROPERTY(EditAnywhere)
		UCameraComponent* MazeCamera;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* PuzzleLight;

	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> MazeBreakActor;


protected:
	UPROPERTY()
		UMazePuzzleWidget* MazeWidget;

	UPROPERTY()
		FCell CurrentCell;

	UPROPERTY()
		bool bIsAnyAnimationPlaying;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UPROPERTY()
		bool bInPuzzle;

	UPROPERTY()
		FTimerHandle TimerHandle;

public:	
	AMaze();

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;

	void SetupInputComponent();
	
	UFUNCTION()
		void MoveLeft();

	UFUNCTION()
		void MoveRight();

	UFUNCTION()
		void MoveUp();

	UFUNCTION()
		void MoveDown();

	UFUNCTION()
		void AnimationDone();

	UFUNCTION()
		void ReverseAnimationDone();

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnterPuzzle();

	void LeavePuzzle();

	void InteractPuzzle();

	UFUNCTION()
		void ToggleMovementKeyBinds();

	UFUNCTION()
		void WinPuzzle();
};

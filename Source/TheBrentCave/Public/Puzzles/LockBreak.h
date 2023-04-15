// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "TimerManager.h"
#include "TBCCharacter.h"
#include "GameFramework/Actor.h"
#include "LockBreak.generated.h"

UCLASS()
class THEBRENTCAVE_API ALockBreak : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALockBreak();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ChestTopMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ChestBottomMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LockBaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LockDialMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LockHookMesh;

	UPROPERTY(EditAnywhere)
		UCurveFloat* DialCurve;

	UPROPERTY(EditAnywhere)
		UCurveFloat* ResetDialCurve;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* Spotlight;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		USoundCue* OnComboSound;

	UPROPERTY(EditAnywhere)
		USoundCue* TickSound;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> BreakLockActor;

	bool ComboRecentlyPlayed;

	FTimeline DialTimeline;
	FTimeline ResetDialTimeline;

	FTimerHandle ResetCooldown;
	FTimerHandle BreakPuzzleTimer;
	FTimerHandle TickTimer;

	void SetupTimelines();

	UFUNCTION()
		void UpdateTimelineProgress(float Value);

	UFUNCTION()
		void UpdateResetTimelineProgress(float Value);

	void SetupPuzzle();
	void SetupPlayerInput();

	void InteractPuzzle();

	void EnterPuzzle();
	void ExitPuzzle();

	bool inPuzzle;
	bool inOverlap;

	UFUNCTION()
		void ResetLock();

	UFUNCTION()
		void StartPuzzle();

	bool clockwise;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		float PickRange;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int LockLength;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		float TickNoiseDelay;

	int LockLevel;

	TArray<int> LockCode;
	void GenerateLockCode();

	float DialLocation;
	void LockPickCheck();

	void WinPuzzle();

	bool resettingLock;

	void TickNoise();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Misc/FileHelper.h"
#include "Components/TextBlock.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "TBCCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Puzzles/WordSearch/WordSearchWidget.h"
#include "WordSearch.generated.h"

UCLASS()
class THEBRENTCAVE_API AWordSearch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWordSearch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(UWordSearchWidget::Direction direction);

	void EnterPuzzle();

	void ExitPuzzle();

	void Select();

	void SetupInput();
	void InteractPuzzle();
	bool inPuzzle;
	bool inOverlap;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int MinPuzzleSize;
	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int MaxPuzzleSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USceneComponent* Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* puzzleMesh;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* GridWidgetComponent;

	UPROPERTY()
		UWordSearchWidget* GridWidget;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* WordWidgetComponent;

	UPROPERTY()
		UUserWidget* WordWidget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UWordSearchWidget> GridWidgetSubclass;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* Spotlight;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY()
		TArray<FString> WordBankArray;

	UPROPERTY()
		FString ChosenWord;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> breakWordActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* wrongSound;

	UPROPERTY(EditAnywhere)
		UAudioComponent* wrongAudioComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void GenerateWordBank();

	void SetWord();

	int selectedRow;
	int selectedCol;
	UWordSearchWidget::Rotation selectedRotation;
	bool canMoveSelection;

	void WinPuzzle();
	void DestroyPuzzle();
	FTimerHandle breakPuzzle;

};

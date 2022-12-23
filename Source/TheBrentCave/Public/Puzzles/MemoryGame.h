// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "TBCCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MemoryGame.generated.h"

UCLASS()
class THEBRENTCAVE_API AMemoryGame : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMemoryGame();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* memoryBase;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* memoryTopLeft;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* memoryTopRight;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* memoryBottomLeft;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* memoryBottomRight;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		UCameraComponent* MemoryCamera;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* PuzzleLight;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY()
		UUserWidget* InteractWidget;

	void setupInput();

	void enterPuzzle();
	void exitPuzzle();

	bool inMemory;

	UPROPERTY(EditAnywhere, Category="Puzzle")
		int loopAm;

	UPROPERTY(EditAnywhere, Category = "Puzzle")
		float TimerBetween;
	UPROPERTY(EditAnywhere, Category = "Puzzle")
		float NewColorTimer;
	UPROPERTY(EditAnywhere, Category = "Puzzle")
		float ResetTimer;
	UPROPERTY(EditAnywhere, Category = "Puzzle")
		float GlowStrength;

	UPROPERTY(EditAnywhere, Category = "Puzzle")
		TSubclassOf<AActor> breakMemoryActor;

	bool inOverlap;

	TArray<int32> pattern;
	int random;

	bool showingCombo;
	int selected;

	void displayNextIndex();
	void pauseTimer();
	void wrongTimer();
	void breakTimer();

	//Audio files.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* topLeftSound;

	UAudioComponent* topLeftAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* topRightSound;

	UAudioComponent* topRightAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* bottomLeftSound;

	UAudioComponent* bottomLeftAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* bottomRightSound;

	UAudioComponent* bottomRightAudioComponent;



	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* selectedSound;

	UAudioComponent* selectedAudioComponent;*/



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* pressedSound;

	UAudioComponent* pressedAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* wrongSound;

	UAudioComponent* wrongAudioComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMaterialInterface* tempMat;
	UMaterialInstanceDynamic* topLeftMat;
	UMaterialInstanceDynamic* topRightMat;
	UMaterialInstanceDynamic* bottomLeftMat;
	UMaterialInstanceDynamic* bottomRightMat;

	int currentIndex;

	FTimerHandle timerHandle;
	FTimerHandle timerPause;
	FTimerHandle timerWrong;
	FTimerHandle timerBreak;

	bool isDestroyCalled;

	//Determines which part of the puzzle you are on, i.e: If there are 3 indexs and you are on the 2nd part of the memory game, currentPart should be set to 2.
	int currentPart;

	//Determines the amount of indexs the showingCombo should display depending on how many times you have answered correctly.
	int displayAm;

};

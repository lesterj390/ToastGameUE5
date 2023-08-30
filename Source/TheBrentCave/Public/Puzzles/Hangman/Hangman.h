// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "TBCCharacter.h"
#include "Puzzles/Hangman/HangmanWidget.h"
#include "Puzzles/Hangman/HangmanInputWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Actor.h"
#include "Hangman.generated.h"

UCLASS()
class THEBRENTCAVE_API AHangman : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHangman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Hook;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Head;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Body;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftArm;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightArm;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftLeg;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightLeg;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* PuzzleLight;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	TArray<FString> sentences;
	FString ChosenSentence;

	bool inOverlap;

	int guesses;

	// Functions
	void Interact();
	void SetupInput();
	void GenerateWidget();
	void GenerateSentences();

	void WrongGuess();
	bool GuessedCorrect(char PGuess);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
		bool inPuzzle;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* ChosenSentenceComponent;

	UPROPERTY()
		UHangmanWidget* ChosenSentenceWidget;

	UPROPERTY()
		UHangmanInputWidget* HangmanInputWidget;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void ExitPuzzle();

	UFUNCTION(BlueprintCallable)
		void GuessCharacter(FString PGuess);

};

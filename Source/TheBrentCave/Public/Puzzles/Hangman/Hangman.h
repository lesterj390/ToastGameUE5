// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/WidgetComponent.h"
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

	TArray<FString> sentences;
	FString ChosenSentence;

	// Functions
	void GenerateSentences();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* ChosenSentenceComponent;

	UPROPERTY()
		UUserWidget* ChosenSentenceWidget;
};

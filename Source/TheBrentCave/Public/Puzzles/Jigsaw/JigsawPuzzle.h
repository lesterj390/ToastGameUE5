// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "JigsawPuzzle.generated.h"

UCLASS()
class THEBRENTCAVE_API AJigsawPuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJigsawPuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
		USpotLightComponent* PuzzleLight;

	UPROPERTY(VisibleAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere, Category = "Puzzle Data")
		int PuzzleSize;

	void GeneratePieces();

};

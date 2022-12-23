// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JigsawPiece.generated.h"

UCLASS()
class THEBRENTCAVE_API AJigsawPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJigsawPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Piece;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		TSubclassOf<UStaticMesh> Fill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		TSubclassOf<UStaticMesh> Extrude;

};

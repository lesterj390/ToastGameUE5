// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Jigsaw/JigsawPuzzle.h"

// Sets default values
AJigsawPuzzle::AJigsawPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("KeyFrame Camera"));
	PuzzleCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AJigsawPuzzle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJigsawPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJigsawPuzzle::GeneratePieces()
{



}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/PipeMatch.h"

// Sets default values
APipeMatch::APipeMatch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	PuzzleBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Puzzle Base"));
	PuzzleBase->SetupAttachment(Pivot);

	NailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Nail Mesh"));
	NailMesh->SetupAttachment(Pivot);

}

// Called when the game starts or when spawned
void APipeMatch::BeginPlay()
{
	Super::BeginPlay();

	PieceSpacing = GetActorScale3D().X / PuzzleSize;
	
	SetupPath();
	GeneratePieces();

	FVector BoxExtent = FVector();
	FVector Origin = FVector();

	// Getting Bounding Info

	AActor* TestPipe = GetWorld()->SpawnActor<AActor>(Straight, Pivot->GetComponentLocation(), GetActorRotation());
	TestPipe->SetActorScale3D(GetActorScale3D() / PuzzleSize);
	
	TestPipe->GetActorBounds(true, Origin, BoxExtent, false);
	PieceSpacing = BoxExtent.Z * 2;
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Transform %f"), PieceSpacing));
	

}

// Called every frame
void APipeMatch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}


void APipeMatch::SetupPath()
{
	FCell currentPosition;
	int row = 0;
	int col = 0;

	// Use a recursive backtracking method of coming up with a set up FCells that lead from the start to the end
	while (!pipePath.Contains(FCell(PuzzleSize - 1, PuzzleSize - 1))) {
		// If next path has no neighbors, run if statement
		currentPosition = nextPath(currentPosition[0], currentPosition[1]);
		if (currentPosition == FCell(-1, -1) && pipePath.Num() > 0) {
			currentPosition = pipePath.Pop();
		}
	}

	// Creates a string that represents the win path for testing purposes
	FString myString = FString(TEXT(""));
	for (FCell currentCell : pipePath) {
		myString += currentCell.ToString() + " ";
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%s"), *myString));
}

FCell APipeMatch::nextPath(int row, int col)
{
	TArray<FCell> neighbors;
	FCell currentCell;

	// Up
	if (row - 1 >= 0) {
		currentCell = FCell(row - 1, col);
		if (!visitedPath.Contains(currentCell)) {
			neighbors.Add(currentCell);
		}
	}
	// Down
	if (row + 1 < PuzzleSize) {
		currentCell = FCell(row + 1, col);
		if (!visitedPath.Contains(currentCell)) {
			neighbors.Add(currentCell);
		}
	}
	// Left
	if (col - 1 >= 0) {
		currentCell = FCell(row, col - 1);
		if (!visitedPath.Contains(currentCell)) {
			neighbors.Add(currentCell);
		}
	}
	// Right
	if (col + 1 < PuzzleSize) {
		currentCell = FCell(row, col + 1);
		if (!visitedPath.Contains(currentCell)) {
			neighbors.Add(currentCell);
		}
	}

	if (neighbors.Num() > 0) {
		currentCell = neighbors[FMath::RandRange(0, neighbors.Num() - 1)];
		pipePath.Push(currentCell);
		visitedPath.Add(currentCell);

		return currentCell;
	}
	
	return FCell(-1, -1);
}

void APipeMatch::GeneratePieces()
{
	// Create starting pipe indicating where the first pipe to match is
	AActor* StartPipeActor = GetWorld()->SpawnActor<AActor>(StartPipe, Pivot->GetComponentLocation(), GetActorRotation());
	StartPipeActor->SetActorRelativeScale3D(FVector(GetActorRelativeScale3D() / PuzzleSize));

	

}
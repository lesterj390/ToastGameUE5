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

	inPuzzle = true;
}

// Called when the game starts or when spawned
void APipeMatch::BeginPlay()
{
	Super::BeginPlay();

	PieceSpacing = GetActorScale3D().X / PuzzleSize;

	// Initializing PipePieces array
	PipePieces.SetNum(PuzzleSize);
	for (int row = 0; row < PuzzleSize; row++) {
		PipePieces[row].SetNum(PuzzleSize);
	}
	
	SetupPath();
	GeneratePieces();
}

// Called every frame
void APipeMatch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GetInput();
}

/*
* Uses recursive backtracking to find a path from point A to B
*/
void APipeMatch::SetupPath()
{
	FCell currentPosition;
	FCell nextPosition;
	int row = 0;
	int col = 0;

	// Adding start pipe to the path first
	pipePath.Add(FCell(-1, 0));

	// Adding the first row first column pipe
	pipePath.Add(FCell(0, 0));
	visitedPath.Add(FCell(0, 0));

	// Use a recursive backtracking method of coming up with a set up FCells that lead from the start to the end
	while (!pipePath.Contains(FCell(PuzzleSize - 1, PuzzleSize - 1))) {
		currentPosition = pipePath.Top();
		nextPosition = GetUnvisitedNeighbour(currentPosition);
		// If currenPosition has an unvisited neighbour
		if (nextPosition != FCell(-1, -1)) {
			pipePath.Push(nextPosition);
			visitedPath.Add(nextPosition);
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Pushed %s"), *(nextPosition.ToString())));
		}
		else {
			pipePath.Pop();
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Popped %s"), *(currentPosition.ToString())));
		}
	}

	// Creates a string that represents the win path for testing purposes
	FString myString = FString(TEXT(""));
	for (FCell currentCell : pipePath) {
		myString += currentCell.ToString() + " ";
	}

	// Adding end pipe to the path
	pipePath.Add(FCell(PuzzleSize, PuzzleSize - 1));
	myString += FCell(PuzzleSize, PuzzleSize - 1).ToString();

	//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("%s"), *myString));
}

/*
* Returns a cells random unvisited neighbour if it has any.
* If not, it returns FCell(-1, -1)
*/
FCell APipeMatch::GetUnvisitedNeighbour(FCell cell)
{
	int row = cell[0];
	int col = cell[1];

	TArray<FCell> neighbors;
	FCell currentCell;

	// Up
	if (row - 1 >= 0) {
		currentCell = FCell(row - 1, col);
		if (!visitedPath.Contains(FCell(row - 1, col))) {
			neighbors.Add(currentCell);
		}
	}
	// Down
	if (row + 1 < PuzzleSize) {
		currentCell = FCell(row + 1, col);
		if (!visitedPath.Contains(FCell(row + 1, col))) {
			neighbors.Add(currentCell);
		}
	}
	// Left
	if (col - 1 >= 0) {
		currentCell = FCell(row, col - 1);
		if (!visitedPath.Contains(FCell(row, col - 1))) {
			neighbors.Add(currentCell);
		}
	}
	// Right
	if (col + 1 < PuzzleSize) {
		currentCell = FCell(row, col + 1);
		if (!visitedPath.Contains(FCell(row, col + 1))) {
			neighbors.Add(currentCell);
		}
	}

	if (neighbors.Num() > 0) {
		currentCell = neighbors[FMath::RandRange(0, neighbors.Num() - 1)];
		return currentCell;
	}
	
	return FCell(-1, -1);
}

/*
* Creates a single pipe based on the pipe type parameter.
* It places this pipe in the correct world location based on a row and column grid system.
* The piece may be randomly rotated if the randomRotation boolean is true.
*/
AActor* APipeMatch::CreatePipe(int row, int col, TSubclassOf<AActor> pipeType, bool randomRotation = true)
{
	int rotations[4] = { 0, 90, 180, 270 };

	FVector currentLocation = startPipeLocation;
	FRotator currentRotation = GetActorRotation();
	FTransform spawnTransform = FTransform();

	
	// Rotating the location vector to disregard the actor's initial rotation
	currentLocation = currentLocation.RotateAngleAxis(GetActorRotation().Yaw * -1, FVector(0, 0, 1));
	// Offsetting initial location
	currentLocation.X += PieceSpacing * col;
	currentLocation.Z -= PieceSpacing * row;
	// Randomly rotating
	currentRotation.Pitch = rotations[FMath::RandRange(0, 3)];
	// Reversing the rotation
	currentLocation = currentLocation.RotateAngleAxis(GetActorRotation().Yaw, FVector(0, 0, 1));
	spawnTransform.SetLocation(currentLocation);
	if (randomRotation) {
		spawnTransform.SetRotation(currentRotation.Quaternion());
	}
	else {
		spawnTransform.SetRotation(GetActorRotation().Quaternion());
	}
	spawnTransform.SetScale3D(FVector(GetActorScale3D() / PuzzleSize));

	AActor* currentPipe = GetWorld()->SpawnActor<AActor>(pipeType, spawnTransform);
	currentPipe->SetActorScale3D(FVector(GetActorScale3D() / PuzzleSize));
	return currentPipe;
}

/*
* Generates the physical pipe pieces based on pipePath
*/
void APipeMatch::GeneratePieces()
{
	FVector BoxExtent = FVector();
	FVector Origin = FVector();
	FVector Location = FVector();
	FTransform SpawnTransform = FTransform();

	// Spawning a temporary pipe to get information on the pipe's start location and the Piece spacing

	// Getting Bounding Info

	SpawnTransform.SetScale3D(FVector(1 / PuzzleSize));
	SpawnTransform.SetLocation(Pivot->GetComponentLocation());
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	AActor* TempPipe = GetWorld()->SpawnActor<AActor>(Straight, SpawnTransform);
	TempPipe->SetActorScale3D(GetActorScale3D() / PuzzleSize);

	TempPipe->GetActorBounds(true, Origin, BoxExtent, false);
	PieceSpacing = BoxExtent.Z * 2;

	Location = TempPipe->GetActorLocation();
	// Rotating the location vector to disregard the actor's initial rotation
	Location = Location.RotateAngleAxis(GetActorRotation().Yaw * -1, FVector(0, 0, 1));
	// Offsetting pipe to row 0 column 0
	Location.X -= PieceSpacing * (PuzzleSize - 1) / 2;
	Location.Z += PieceSpacing * (PuzzleSize - 1) / 2;
	// Reverse the rotation
	Location = Location.RotateAngleAxis(GetActorRotation().Yaw, FVector(0, 0, 1));
	startPipeLocation = Location;
	
	// Removing temp pipe after location and piece spacing are calculated
	TempPipe->Destroy();


	// Creates start pipe
	CreatePipe(0, 0, StartPipe, false);

	// Creates the pipes for the pipe path
	for (int i = 1; i < pipePath.Num() - 1; i++) {

		FCell currentPipe = pipePath[i];
		int currentRow = currentPipe[0];
		int currentCol = currentPipe[1];

		// Straight horizontal pipe
		if (pipePath[i - 1][0] == pipePath[i + 1][0]) {
			PipePieces[currentRow][currentCol] = CreatePipe(currentRow, currentCol, Straight);
		}
		// Straight vertical pipe
		else if (pipePath[i - 1][1] == pipePath[i + 1][1]) {
			PipePieces[currentRow][currentCol] = CreatePipe(currentRow, currentCol, Straight);
		}
		// Angled pipe
		else {
			PipePieces[currentRow][currentCol] = CreatePipe(currentRow, currentCol, Angle);
		}
	}

	// Creates the occasional random pipe that's not on the path to throw you off
	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (!pipePath.Contains(FCell(row, col))) {
				int randomNumber = FMath::RandRange(1, 100);
				// 60 percent chance pipe spawns
				if (randomNumber <= 60) {
					// 50 percent chance for straight pipe
					if (FMath::RandRange(1, 2) == 1) {
						PipePieces[row][col] = CreatePipe(row, col, Straight);
					}
					// 50 percent chance for angle pipe
					else {
						PipePieces[row][col] = CreatePipe(row, col, Angle);
					}
				}
				else {
					PipePieces[row][col] = NULL;
				}
			}
		}
	}

	// Creates end pipe
	CreatePipe(PuzzleSize - 1, PuzzleSize - 1, EndPipe, false);
}

void APipeMatch::GetInput()
{
	if (inPuzzle) {
		if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left)) {
			// Move selected pipe piece one to the left, if the selected col is > than 0
			if (selectedPipe[1] > 0) {
				selectedPipe[1]--;
				GetSelected();
			}
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right)) {
			// Move selected pipe piece one to the right, if the selected col is < than PuzzleSize
			if (selectedPipe[1] < PuzzleSize - 1) {
				selectedPipe[1]++;
				GetSelected();
			}
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up)) {
			// Move selected pipe piece one up, if the selected col is > than 0
			if (selectedPipe[0] > 0) {
				selectedPipe[0]--;
				GetSelected();
			}
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down)) {
			// Move selected pipe piece one down, if the selected col is < than PuzzleSize
			if (selectedPipe[0] < PuzzleSize - 1) {
				selectedPipe[0]++;
				GetSelected();
			}
		}
	}
}

void APipeMatch::GetSelected()
{
	UStaticMeshComponent* PipeMesh;
	TArray<UActorComponent*> FoundComponents;
	PipePieces[selectedPipe[0]][selectedPipe[1]]->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
	PipeMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

	tempMat = PipeMesh->GetMaterial(0);
	PipeMat = UMaterialInstanceDynamic::Create(tempMat, this);
	PipeMesh->SetMaterial(0, PipeMat);

	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (selectedPipe == FCell(row, col)) {
				PipeMat->SetScalarParameterValue(TEXT("Glow"), 0.5);
			}
			else {
				PipeMat->SetScalarParameterValue(TEXT("Glow"), 0);
			}
		}
	}
}
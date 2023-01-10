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

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(Pivot);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PuzzleCamera->SetupAttachment(Pivot);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(Pivot);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(Pivot);

	inOverlap = false;
	inPuzzle = false;
}

// Called when the game starts or when spawned
void APipeMatch::BeginPlay()
{
	Super::BeginPlay();

	PuzzleSize = FMath::RandRange(MinPuzzleSize, MaxPuzzleSize);

	Spotlight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

	PieceSpacing = GetActorScale3D().X / PuzzleSize;

	// Initializing PipePieces array
	PipePieces.SetNum(PuzzleSize);
	for (int row = 0; row < PuzzleSize; row++) {
		PipePieces[row].SetNum(PuzzleSize);
	}
	
	SetupPath();
	GeneratePieces();

	selectedPipe = FCell(0, 0);

	SetupKeybinds();
}

// Called every frame
void APipeMatch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (inPuzzle) {
		GetInput();
	}
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

void APipeMatch::SetupKeybinds()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &APipeMatch::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &APipeMatch::OnOverlapEnd);

	InputComponent->BindAction("Interact", IE_Pressed, this, &APipeMatch::Interact);
	InputComponent->BindAction("Exit", IE_Pressed, this, &APipeMatch::ExitPuzzle);
	DisableInput(GetWorld()->GetFirstPlayerController());


	DisableInput(GetWorld()->GetFirstPlayerController());
}

void APipeMatch::Interact()
{
	//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Interact function called")));

	if (inPuzzle) {
		RotateSelectedPipe();
		if (CheckForWin()) {
			ExitPuzzle();
			GetWorldTimerManager().SetTimer(breakPuzzle, this, &APipeMatch::WinPuzzle, 0.6, false);
		}
	}
	else {
		EnterPuzzle();
	}
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
	CornerPipes.Add(CreatePipe(0, 0, StartPipe, false));

	// Creates the pipes for the pipe path
	FCell lastPipe = FCell(0, 0);
	int lastRow = 0;
	int lastCol = 0;
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

		// Setting pipes input
		// Left
		if (currentRow == lastRow && currentCol - 1 == lastCol) {
			PipePieces[currentRow][currentCol]->Tags.Add(FName("LEFT"));
		}
		// Right
		else if (currentRow == lastRow && currentCol + 1 == lastCol) {
			PipePieces[currentRow][currentCol]->Tags.Add(FName("RIGHT"));
		}
		// Up
		else if (currentRow - 1 == lastRow && currentCol == lastCol) {
			PipePieces[currentRow][currentCol]->Tags.Add(FName("UP"));
		}
		//Down
		else if (currentRow + 1 == lastRow && currentCol == lastCol) {
			PipePieces[currentRow][currentCol]->Tags.Add(FName("DOWN"));
		}

		lastPipe = currentPipe;
		lastRow = currentRow;
		lastCol = currentCol;
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
	CornerPipes.Add(CreatePipe(PuzzleSize - 1, PuzzleSize - 1, EndPipe, false));

	SetupMaterials();
	GetSelected();
}

void APipeMatch::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATBCCharacter* OtherTBCCharacter = Cast<ATBCCharacter>(OtherActor);
	if (OtherTBCCharacter) {
		if (OtherTBCCharacter->Tags.Num() > 0)
		{

			FName PlayerTag = OtherTBCCharacter->Tags[0];
			TArray<AActor*> FoundActors;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), PlayerTag, FoundActors);

			if (FoundActors.Num() > 0)
			{
				EnableInput(GetWorld()->GetFirstPlayerController());
				InteractComponent->SetVisibility(true);

				HitBoxPlayer = Cast<ATBCCharacter>(FoundActors[0]);
				inOverlap = true;
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
			}
		}
	}
}

void APipeMatch::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Left Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
	}
}

void APipeMatch::GetInput()
{
	if (inPuzzle) {
		// Move selected pipe piece one to the left, if the selected col is > than 0
		if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left)) {
			FCell lastSelected = selectedPipe;
			// Go to the next non-null cell on the left if there is any
			while (selectedPipe[1] > 0) {
				// If the cell is not empty
				selectedPipe[1]--;
				if (PipePieces[selectedPipe[0]][selectedPipe[1]] != NULL) {
					GetSelected();
					break;
				}
				// If every cell to the left of this row is null
				else if (selectedPipe[1] == 0) {
					selectedPipe = lastSelected;
					break;
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Went Left to %s"), *(selectedPipe.ToString())));
		// Move selected pipe piece one to the right, if the selected col is < than PuzzleSize
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right)) {
			FCell lastSelected = selectedPipe;
			// Go to the next non-null cell on the right if there is any
			while (selectedPipe[1] < PuzzleSize - 1) {
				// If the cell is not empty
				selectedPipe[1]++;
				if (PipePieces[selectedPipe[0]][selectedPipe[1]] != NULL) {
					GetSelected();
					break;
				}
				// If every cell to the right of this row is null
				else if (selectedPipe[1] == PuzzleSize - 1) {
					selectedPipe = lastSelected;
					break;
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Went Right to %s"), *(selectedPipe.ToString())));
		// Move selected pipe piece one up, if the selected col is > than 0
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up)) {
			FCell lastSelected = selectedPipe;
			// Go to the next non-null cell above if there is any
			while (selectedPipe[0] > 0) {
				// If the cell is not empty
				selectedPipe[0]--;
				if (PipePieces[selectedPipe[0]][selectedPipe[1]] != NULL) {
					GetSelected();
					break;
				}
				// If every cell above in this column is null
				else if (selectedPipe[0] == 0) {
					selectedPipe = lastSelected;
					break;
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Went Up to %s"), *(selectedPipe.ToString())));
		// Move selected pipe piece one down, if the selected col is < than PuzzleSize
		} else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down)) {
			FCell lastSelected = selectedPipe;
			// Go to the next non-null cell below if there is any
			while (selectedPipe[0] < PuzzleSize - 1) {
				// If the cell is not empty
				selectedPipe[0]++;
				if (PipePieces[selectedPipe[0]][selectedPipe[1]] != NULL) {
					GetSelected();
					break;
				}
				// If every cell below in this column is null
				else if (selectedPipe[0] == PuzzleSize - 1) {
					selectedPipe = lastSelected;
					break;
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Cyan, FString::Printf(TEXT("Went Down to %s"), *(selectedPipe.ToString())));
		}
	}
}

/*
* Replaces all the materials with dynamic material instances so material parameters can be modified
*/
void APipeMatch::SetupMaterials()
{
	UStaticMeshComponent* PipeMesh;
	TArray<UActorComponent*> FoundComponents;

	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (PipePieces[row][col] != NULL) {
				PipePieces[row][col]->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
				PipeMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

				tempMat = PipeMesh->GetMaterial(0);
				PipeMat = UMaterialInstanceDynamic::Create(tempMat, this);
				PipeMesh->SetMaterial(0, PipeMat);
			}
		}
	}
}

int APipeMatch::GetRotation(FCell cell)
{
	AActor* chosenPipe = PipePieces[cell[0]][cell[1]];
	FRotator oldRotation = chosenPipe->GetActorRotation();

	// Rounding every value
	oldRotation.Pitch = FMath::RoundHalfFromZero(oldRotation.Pitch);
	oldRotation.Roll = FMath::RoundHalfFromZero(oldRotation.Roll);
	oldRotation.Yaw = FMath::RoundHalfFromZero(oldRotation.Yaw);

	// Switching values equal to 360 to 0
	if ((int)oldRotation.Roll % 360 == 0) {
		oldRotation.Roll = 0;
	}
	if ((int)oldRotation.Pitch % 360 == 0) {
		oldRotation.Pitch = 0;
	}
	if ((int)oldRotation.Yaw % 360 == 0) {
		oldRotation.Yaw = 0;
	}

	if (!FMath::IsNearlyEqual(oldRotation.Yaw, GetActorRotation().Yaw, 0.1)) {
		oldRotation = oldRotation.GetEquivalentRotator();
	}

	return (int)(360 - oldRotation.Pitch) % 360;
}

void APipeMatch::RotateSelectedPipe()
{
	AActor* chosenPipe = PipePieces[selectedPipe[0]][selectedPipe[1]];

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Old Pitch = %d"), GetRotation(selectedPipe)));
	chosenPipe->AddActorLocalRotation(FRotator(-90, 0, 0));

	// Play Pipe Rotation Sound
	UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), RotateSound);
	SoundComponent->Play(0.0);
}

/*
* Replaces all the materials with dynamic material instances so material parameters can be modified
*/
void APipeMatch::GetSelected()
{
	UStaticMeshComponent* PipeMesh;
	TArray<UActorComponent*> FoundComponents;

	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (PipePieces[row][col] != NULL) {
				PipePieces[row][col]->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
				PipeMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

				tempMat = PipeMesh->GetMaterial(0);
				PipeMat = Cast<UMaterialInstanceDynamic>(tempMat);

				if (inPuzzle) {
					if (selectedPipe == FCell(row, col)) {
						PipeMat->SetScalarParameterValue(TEXT("Glow"), 0.5);
					}
					else {
						PipeMat->SetScalarParameterValue(TEXT("Glow"), 0);
					}
				}
				else {
					PipeMat->SetScalarParameterValue(TEXT("Glow"), 0);
				}
			}
		}
	}
}

/*
* Traverses the current path of the pipes and set's the input tags of said path
*/
void APipeMatch::DiscoverPathInputs()
{
	// Setting the first pipes input to UP
	AActor* currentPipe = PipePieces[0][0];
	FCell currentCell = FCell(0, 0);
	currentPipe->Tags.Empty();
	if (GetPipeDirections(currentCell).Contains(UP)) {
		currentPipe->Tags.Add(FName("UP"));
	}
	else {
		// Can't discover path if the first pipe doesn't have an UP input
		return;
	}
	direction currentOutput = GetPipeOutput(currentCell);
	TArray<FCell> visited;
	visited.Add(currentCell);

	while (currentOutput != NONE) {
		if (currentOutput == UP) {
			currentCell[0] -= 1;
			if (currentCell[0] < 0 || PipePieces[currentCell[0]][currentCell[1]] == nullptr) {
				break;
			}
			currentPipe = PipePieces[currentCell[0]][currentCell[1]];
			currentPipe->Tags.Empty();
			if (GetPipeDirections(currentCell).Contains(DOWN)) {
				currentPipe->Tags.Add(FName("DOWN"));
			}
			else {
				break;
			}
		}
		else if (currentOutput == DOWN) {
			currentCell[0] += 1;
			if (currentCell[0] >= PuzzleSize || PipePieces[currentCell[0]][currentCell[1]] == nullptr) {
				break;
			}
			currentPipe = PipePieces[currentCell[0]][currentCell[1]];
			currentPipe->Tags.Empty();
			if (GetPipeDirections(currentCell).Contains(UP)) {
				currentPipe->Tags.Add(FName("UP"));
			}
			else {
				break;
			}
		}
		else if (currentOutput == LEFT) {
			currentCell[1] -= 1;
			if (currentCell[1] < 0 || PipePieces[currentCell[0]][currentCell[1]] == nullptr) {
				break;
			}
			currentPipe = PipePieces[currentCell[0]][currentCell[1]];
			currentPipe->Tags.Empty();
			if (GetPipeDirections(currentCell).Contains(RIGHT)) {
				currentPipe->Tags.Add(FName("RIGHT"));
			}
			else {
				break;
			}
		}
		else if (currentOutput == RIGHT) {
			currentCell[1] += 1;
			if (currentCell[1] >= PuzzleSize || PipePieces[currentCell[0]][currentCell[1]] == nullptr) {
				break;
			}
			currentPipe = PipePieces[currentCell[0]][currentCell[1]];
			currentPipe->Tags.Empty();
			if (GetPipeDirections(currentCell).Contains(LEFT)) {
				currentPipe->Tags.Add(FName("LEFT"));
			}
			else {
				break;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Current Cell = %s"), *currentCell.ToString()), false);
		currentOutput = GetPipeOutput(currentCell);
		
		// Avoids infinite pipe loops
		if (visited.Contains(currentCell)) {
			break;
		}
		else {
			visited.Add(currentCell);
		}
	}
}


bool APipeMatch::PipeInBounds(FCell pipe)
{
	int row = pipe[0];
	int col = pipe[1];

	if (row < 0 || row >= PuzzleSize) {
		return false;
	}
	if (col < 0 || col >= PuzzleSize) {
		return false;
	}
	return true;
}

FString APipeMatch::directionToString(direction d)
{
	if (d == UP) {
		return FString("Up");
	}
	else if (d == DOWN) {
		return FString("Down");
	}
	else if (d == LEFT) {
		return FString("Left");
	}
	else if (d == RIGHT) {
		return FString("Right");
	}
	return FString("None");
}

/*
* Gets the two pipe directions and returns them in a TArray
*/
TArray<APipeMatch::direction> APipeMatch::GetPipeDirections(FCell pipe)
{
	AActor* pipeActor = PipePieces[pipe[0]][pipe[1]];
	int pipeRotation = GetRotation(pipe);
	TArray<direction> directions;

	if (pipeActor->GetClass()->IsChildOf(Straight)) {
		// Pipe is vertical
		if (pipeRotation == 0 || pipeRotation == 180) {
			directions.Add(UP);
			directions.Add(DOWN);
		}
		// Pipe is horizontal
		else if (pipeRotation == 90 || pipeRotation == 270) {
			directions.Add(LEFT);
			directions.Add(RIGHT);
		}

	}
	// The current pipe is angled
	else if (pipeActor->GetClass()->IsChildOf(Angle)) {
		if (pipeRotation == 0) {
			directions.Add(UP);
			directions.Add(RIGHT);
		}
		else if (pipeRotation == 90) {
			directions.Add(RIGHT);
			directions.Add(DOWN);
		}
		else if (pipeRotation == 180) {
			directions.Add(DOWN);
			directions.Add(LEFT);
		}
		else if (pipeRotation == 270) {
			directions.Add(LEFT);
			directions.Add(UP);
		}
	}
	
	return directions;
}

/*
* Returns the input direction of the pipe
*/
APipeMatch::direction APipeMatch::GetPipeInput(FCell pipe)
{
	if (!PipeInBounds(pipe)) {
		return NONE;
	}

	AActor* pipeActor = PipePieces[pipe[0]][pipe[1]];
	if (pipeActor->Tags.Num() > 0) {
		FName directionName = pipeActor->Tags[0];
		if (directionName.IsEqual(FName("UP"))) {
			return UP;
		}
		else if (directionName.IsEqual(FName("DOWN"))) {
			return DOWN;
		}
		else if (directionName.IsEqual(FName("LEFT"))) {
			return LEFT;
		}
		else if (directionName.IsEqual(FName("RIGHT"))) {
			return RIGHT;
		}
	}
	return NONE;
}

/*
* Returns the output direction of the pipe
*/
APipeMatch::direction APipeMatch::GetPipeOutput(FCell pipe)
{
	if (!PipeInBounds(pipe) || PipePieces[pipe[0]][pipe[1]] == nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Cell %s null or out of bounds"), *(pipe.ToString())), false);
		return NONE;
	}
	int pipeRotation = GetRotation(pipe);
	TArray<direction> directions = GetPipeDirections(pipe);

	// Remove the pipeActor's input direction
	directions.Remove(GetPipeInput(pipe));

	// If there is only one directions left / the pipe had an input
	if (directions.Num() == 1) {
		// Return the remaining output direction
		return directions[0];
	}
	else {
		return NONE;
	}

}

/*
* Returns true if the top left pipe leads to the bottom right pipe
* False otherwise
*/
bool APipeMatch::CheckForWin()
{
	DiscoverPathInputs();

	FCell currentCell = FCell(0, 0);
	//AActor* currentCellActor = PipePieces[currentCell[0]][currentCell[1]];
	direction currentOutput = UP;


	while (currentOutput != NONE) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Cell %s"), *(currentCell.ToString())), false);
		currentOutput = GetPipeOutput(currentCell);

		// Move Up
		if (currentOutput == UP) {
			currentCell[0] -= 1;
		}
		// Move Down
		else if (currentOutput == DOWN) {
			currentCell[0] += 1;
		}
		// Move Left
		else if (currentOutput == LEFT) {
			currentCell[1] -= 1;
		}
		// Move Right
		else if (currentOutput == RIGHT) {
			currentCell[1] += 1;
		}
	}

	if (currentCell == FCell(PuzzleSize, PuzzleSize - 1)) {
		return true;
	}
	else {
		return false;
	}
}

void APipeMatch::EnterPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = true;

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
		InteractComponent->SetVisibility(false);
		Spotlight->SetVisibility(true);

		HitBoxPlayer->SetActorHiddenInGame(true);

		GetSelected();
	}
}

void APipeMatch::ExitPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = false;

		Spotlight->SetVisibility(false);
		HitBoxPlayer->SetActorHiddenInGame(false);
		InteractComponent->SetVisibility(true);

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);

		GetSelected();
	}
}

void APipeMatch::WinPuzzle()
{
	// Destroy all Pipe Piece Actors
	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) 
		{
			if (PipePieces[row][col] != NULL) {
				PipePieces[row][col]->Destroy();
			}
		}
	}

	// Destroy Start and End Pipes
	for (int i = 0; i < CornerPipes.Num(); i++) {
		CornerPipes[i]->Destroy();
	}

	// Spawn Destroy Puzzle Actor
	AActor* breakPipePuzzle = GetWorld()->SpawnActor<AActor>(breakPipeActor, PuzzleBase->GetComponentLocation(), GetActorRotation());

	// Destroy this
	Destroy();
}
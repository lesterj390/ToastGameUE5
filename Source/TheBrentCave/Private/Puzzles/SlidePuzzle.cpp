// Don't steal out game please :( we worked hard on it!


#include "Puzzles/SlidePuzzle.h"

// Sets default values
ASlidePuzzle::ASlidePuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	PuzzleBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Puzzle Base"));
	PuzzleBase->SetupAttachment(Pivot);

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
void ASlidePuzzle::BeginPlay()
{
	Super::BeginPlay();

	selectedPiece.Add(0);
	selectedPiece.Add(0);

	Spotlight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

	CompleteImageWidget = CreateWidget<UUserWidget>(GetWorld(), CompleteImageWidgetClass);

	SetupInput();

	GeneratePieces();
}

void ASlidePuzzle::GeneratePieces()
{
	// Initializing PuzzlePieces array
	PuzzlePieces.SetNum(PuzzleSize);
	for (int row = 0; row < PuzzleSize; row++) {
		PuzzlePieces[row].SetNum(PuzzleSize);
	}

	FVector BoxExtent = FVector();
	FVector Origin = FVector();
	FVector Location = FVector(0);
	FTransform SpawnTransform = FTransform();

	SpawnTransform.SetScale3D(FVector(1 / (float)PuzzleSize));
	SpawnTransform.SetLocation(FVector(0, 0, 0));
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	AActor* TempPiece = GetWorld()->SpawnActor<AActor>(PuzzlePiece, SpawnTransform);
	//TempPiece->AttachToComponent(Pivot, FAttachmentTransformRules::KeepRelativeTransform);
	TempPiece->SetActorRelativeScale3D(FVector(1 / (float)PuzzleSize));
	//TempPiece->SetActorRelativeScale3D(GetActorRelativeScale3D() / PuzzleSize);

	TempPiece->GetActorBounds(true, Origin, BoxExtent, false);
	PieceSpacing = BoxExtent.Z * 2;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%f"), PieceSpacing));

	// Offsetting puzzle piece to row 0 column 0
	Location.X -= PieceSpacing * (PuzzleSize - 1) / 2.0;
	Location.Z += PieceSpacing * (PuzzleSize - 1) / 2.0;
	
	startPieceLocation = Location;

	// Removing temp puzzle piece after location and piece spacing are calculated
	TempPiece->Destroy();

	// Get missing piece value
	randomMissing.Add(FMath::RandRange(0, PuzzleSize - 1));
	randomMissing.Add(FMath::RandRange(0, PuzzleSize - 1));
	
	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			// Skip generation for one random piece
			if (randomMissing[0] == row && randomMissing[1] == col) {
				PuzzlePieces[row][col] = NULL;

				continue;
			}

			// Set the first generated puzzle piece to the default selected piece
			if (selectedPiece[0] == -1 && selectedPiece[1] == -1) {
				selectedPiece[0] = row;
				selectedPiece[1] = col;
			}

			float pieceZ = Location.Z + (row * -1) * PieceSpacing;
			float pieceX = Location.X + col * PieceSpacing;

			PuzzlePieces[row][col] = GetWorld()->SpawnActor<AActor>(PuzzlePiece, FVector(Pivot->GetComponentLocation()), FRotator(0, 0, 0));
			PuzzlePieces[row][col]->AttachToComponent(Pivot, FAttachmentTransformRules::KeepRelativeTransform);
			PuzzlePieces[row][col]->SetActorRelativeScale3D(FVector(1 / (float)PuzzleSize));
			PuzzlePieces[row][col]->SetActorRelativeLocation(FVector(pieceX, 0, pieceZ));

			// Tag current puzzle piece with its home position
			PuzzlePieces[row][col]->Tags.Add(FName(*FString::Printf(TEXT("%d,%d"), row, col)));

			// Call function to set pieces material / texture
			SetPieceTexture(row, col);
		}
	}

	ShufflePieces();
}

void ASlidePuzzle::SetPieceTexture(int row, int col)
{
	PuzzleImage = PuzzleImageArray[FMath::RandRange(0, PuzzleImageArray.Num() - 1)];

	if (PuzzlePieces[row][col]) {
		if (PuzzlePieces[row][col]) {
			UStaticMeshComponent* PieceMesh;
			TArray<UActorComponent*> FoundComponents;
			AActor* currentPiece = PuzzlePieces[row][col];

			// Get reference to slide piece material
			currentPiece->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
			PieceMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

			// Set slide piece material to a dynamic material
			UMaterialInterface* tempMaterial = PieceMesh->GetMaterial(0);
			PuzzleMat = UMaterialInstanceDynamic::Create(tempMaterial, this);
			PieceMesh->SetMaterial(0, PuzzleMat);

			// Define variables as floats
			float fSize = PuzzleSize;
			float fRow = row;
			float fCol = col;

			// Apply image to slide piece
			PuzzleMat->SetTextureParameterValue(TEXT("SlideImage"), PuzzleImage);
			PuzzleMat->SetScalarParameterValue(TEXT("Size"), fSize);
			PuzzleMat->SetScalarParameterValue(TEXT("CustomV"), fRow);
			PuzzleMat->SetScalarParameterValue(TEXT("CustomU"), fCol);
		}
	}
}

void ASlidePuzzle::ShufflePieces()
{
	for (int i = 0; i < PuzzleSize * 50; i++) {
		for (int row = 0; row < PuzzleSize; row++) {
			for (int col = 0; col < PuzzleSize; col++) {
				if (PuzzlePieces[row][col] == NULL) {
					GetValidMovements(row, col);
				}
			}
		}
	}

	selectedPiece[0] = 0;
	selectedPiece[1] = 0;

	if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL) {
		selectedPiece[1]++;
	}
}

void ASlidePuzzle::GetValidMovements(int row, int col)
{
	TArray<int> ValidDirections;

	if (row - 1 >= 0) {
		// Can go Up
		ValidDirections.Add(UP);
	}
	if (row + 1 < PuzzleSize) {
		// Can go Down
		ValidDirections.Add(DOWN);
	}
	if (col - 1 >= 0) {
		// Can go Left
		ValidDirections.Add(LEFT);
	}
	if (col + 1 < PuzzleSize) {
		// Can go Right
		ValidDirections.Add(RIGHT);
	}

	int randDirection = ValidDirections[FMath::RandRange(0, ValidDirections.Num() - 1)];
	if (randDirection == UP) {
		selectedPiece[0] = row - 1;
		selectedPiece[1] = col;
	}
	else if (randDirection == DOWN) {
		selectedPiece[0] = row + 1;
		selectedPiece[1] = col;
	}
	else if (randDirection == LEFT) {
		selectedPiece[0] = row;
		selectedPiece[1] = col - 1;
	}
	else if (randDirection == RIGHT) {
		selectedPiece[0] = row;
		selectedPiece[1] = col + 1;
	}

	MovePiece(true);
}

void ASlidePuzzle::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ASlidePuzzle::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Left Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
	}
}

void ASlidePuzzle::EnterPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = true;

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
		InteractComponent->SetVisibility(false);
		Spotlight->SetVisibility(true);

		HitBoxPlayer->SetActorHiddenInGame(true);
		HitBoxPlayer->SetActorEnableCollision(false);

		EnableInput(GetWorld()->GetFirstPlayerController());
		inOverlap = true;

		DisplaySelectedPiece();
	}
}

void ASlidePuzzle::ExitPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = false;

		Spotlight->SetVisibility(false);
		HitBoxPlayer->SetActorHiddenInGame(false);
		HitBoxPlayer->SetActorEnableCollision(true);
		InteractComponent->SetVisibility(true);

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);
	}
}

void ASlidePuzzle::SetupInput()
{
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &ASlidePuzzle::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &ASlidePuzzle::OnOverlapEnd);

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &ASlidePuzzle::Interact);
	DECLARE_DELEGATE_OneParam(UseDelegate, bool);
	InputComponent->BindAction<UseDelegate>("Use", IE_Pressed, this, &ASlidePuzzle::MovePiece, false);
}

void ASlidePuzzle::Interact()
{
	if (inOverlap) {
		if (!inPuzzle) {
			EnterPuzzle();
		} else {
			ExitPuzzle();
		}
	}
}

void ASlidePuzzle::GetInput()
{
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up)) {
		if (selectedPiece[0] > 0) {
			selectedPiece[0]--;

			if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL && selectedPiece[0] == 0) {
				selectedPiece[0]++;
			}
			else if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL) {
				selectedPiece[0]--;
			}

			DisplaySelectedPiece();
		}
	}
	else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down)) {
		if (selectedPiece[0] < PuzzleSize - 1) {
			selectedPiece[0]++;

			if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL && selectedPiece[0] == PuzzleSize - 1) {
				selectedPiece[0]--;
			}
			else if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL) {
				selectedPiece[0]++;
			}

			DisplaySelectedPiece();
		}
	}
	else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left)) {
		if (selectedPiece[1] > 0) {
			selectedPiece[1]--;

			if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL && selectedPiece[1] == 0) {
				selectedPiece[1]++;
			}
			else if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL) {
				selectedPiece[1]--;
			}

			DisplaySelectedPiece();
		}
	}
	else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right)) {
		if (selectedPiece[1] < PuzzleSize - 1) {
			selectedPiece[1]++;

			if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL && selectedPiece[1] == PuzzleSize - 1) {
				selectedPiece[1]--;
			}
			else if (PuzzlePieces[selectedPiece[0]][selectedPiece[1]] == NULL) {
				selectedPiece[1]++;
			}

			DisplaySelectedPiece();
		}
	}
	else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Tab)) {
		if (CompleteImageWidgetClass)
		{
			if (CompleteImageWidget && CompleteImageWidget->IsInViewport())
			{
				CompleteImageWidget->RemoveFromParent();
			}
			else if (CompleteImageWidget) {
				CompleteImageWidget->AddToViewport();

				UImage* WidgetPuzzleImage = (UImage*)(CompleteImageWidget->WidgetTree->FindWidget(FName("FinalImage")));
				WidgetPuzzleImage->SetBrushFromTexture(Cast<UTexture2D>(PuzzleImage));
			}
		}
	}
}

void ASlidePuzzle::DisplaySelectedPiece()
{
	UStaticMeshComponent* PieceMesh;
	TArray<UActorComponent*> FoundComponents;

	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (PuzzlePieces[row][col] != NULL) {
				PuzzlePieces[row][col]->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
				PieceMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

				tempMat = PieceMesh->GetMaterial(0);
				PieceMat = Cast<UMaterialInstanceDynamic>(tempMat);

				// Take piece home location from tag and convert tag row & col to floats
				FName piecePosition = PuzzlePieces[row][col]->Tags[0];
				FString pieceRow, pieceCol;
				piecePosition.ToString().Split(TEXT(","), &pieceRow, &pieceCol);
				float fRow = FCString::Atof(*pieceRow);
				float fCol = FCString::Atof(*pieceCol);

				// Define variables as floats
				float fSize = PuzzleSize;

				// Apply image to slide piece
				//PuzzleMat->SetTextureParameterValue(TEXT("SlideImage"), PuzzleImage);
				//PuzzleMat->SetScalarParameterValue(TEXT("Size"), fSize);
				//PuzzleMat->SetScalarParameterValue(TEXT("CustomV"), fRow);
				//PuzzleMat->SetScalarParameterValue(TEXT("CustomU"), fCol);

				if (inPuzzle) {
					if (selectedPiece[0] == row && selectedPiece[1] == col) {
						PieceMat->SetScalarParameterValue(TEXT("Glow"), 5);
						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%f , %f"), fRow, fCol));
					}
					else {
						PieceMat->SetScalarParameterValue(TEXT("Glow"), 0);
					}
				}
				else {
					PieceMat->SetScalarParameterValue(TEXT("Glow"), 0);
				}
			}
		}
	}
}

void ASlidePuzzle::MovePiece(bool shuffle)
{
	if (inPuzzle || shuffle) {
		int row = selectedPiece[0];
		int col = selectedPiece[1];

		USceneComponent* PieceComponent;
		TArray<UActorComponent*> FoundComponents;
		AActor* currentPiece = PuzzlePieces[row][col];

		// Get reference to slide piece
		currentPiece->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
		PieceComponent = currentPiece->GetRootComponent();
		FVector PieceLocation = PieceComponent->GetRelativeLocation();

		// Check for empty piece space beside the selected piece
		if (row - 1 >= 0 && PuzzlePieces[row - 1][col] == NULL) {
			// Piece Up is empty

			currentPiece->SetActorRelativeLocation(FVector(PieceLocation.X, PieceLocation.Y, PieceLocation.Z + PieceSpacing));

			PuzzlePieces[row - 1][col] = PuzzlePieces[row][col];
			PuzzlePieces[row][col] = NULL;

			selectedPiece[0] = row - 1;

			if (inPuzzle) {
				UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), SlideSound);
				SoundComponent->Play(0.0);
			}
		}
		else if (row + 1 <= PuzzleSize - 1 && PuzzlePieces[row + 1][col] == NULL) {
			// Piece Down is empty

			currentPiece->SetActorRelativeLocation(FVector(PieceLocation.X, PieceLocation.Y, PieceLocation.Z - PieceSpacing));

			PuzzlePieces[row + 1][col] = PuzzlePieces[row][col];
			PuzzlePieces[row][col] = NULL;

			selectedPiece[0] = row + 1;

			if (inPuzzle) {
				UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), SlideSound);
				SoundComponent->Play(0.0);
			}
		}
		else if (col - 1 >= 0 && PuzzlePieces[row][col - 1] == NULL) {
			// Piece Left is empty

			currentPiece->SetActorRelativeLocation(FVector(PieceLocation.X - PieceSpacing, PieceLocation.Y, PieceLocation.Z));

			PuzzlePieces[row][col - 1] = PuzzlePieces[row][col];
			PuzzlePieces[row][col] = NULL;

			selectedPiece[1] = col - 1;

			if (inPuzzle) {
				UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), SlideSound);
				SoundComponent->Play(0.0);
			}
		}
		else if (col + 1 <= PuzzleSize - 1 && PuzzlePieces[row][col + 1] == NULL) {
			// Piece Right is empty

			currentPiece->SetActorRelativeLocation(FVector(PieceLocation.X + PieceSpacing, PieceLocation.Y, PieceLocation.Z));

			PuzzlePieces[row][col + 1] = PuzzlePieces[row][col];
			PuzzlePieces[row][col] = NULL;

			selectedPiece[1] = col + 1;

			if (inPuzzle) {
				UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), SlideSound);
				SoundComponent->Play(0.0);
			}
		}
	}

	if (inPuzzle) {
		if (CheckForWin()) {
			ExitPuzzle();
			GetWorldTimerManager().SetTimer(breakPuzzle, this, &ASlidePuzzle::WinPuzzle, 0.6, false);
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Wrong!")));
		}
	}
}

bool ASlidePuzzle::CheckForWin()
{
	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			if (PuzzlePieces[row][col] == NULL) {
				continue;
			}

			// Take piece home location from tag and convert tag row & col to ints
			FName piecePosition = PuzzlePieces[row][col]->Tags[0];
			FString pieceRow, pieceCol;
			piecePosition.ToString().Split(TEXT(","), &pieceRow, &pieceCol);
			float iRow = FCString::Atoi(*pieceRow);
			float iCol = FCString::Atoi(*pieceCol);

			if (row != iRow || col != iCol) {
				return false;
			}
		}
	}

	return true;
}

void ASlidePuzzle::WinPuzzle()
{
	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++)
		{
			if (PuzzlePieces[row][col] != NULL) {
				PuzzlePieces[row][col]->Destroy();
			}
		}
	}

	AActor* breakSlidePuzzle = GetWorld()->SpawnActor<AActor>(DestroySlideActor, GetActorLocation(), GetActorRotation());
	breakSlidePuzzle->SetActorScale3D(FVector(GetActorScale3D()));
	
	Destroy();
}

// Called every frame
void ASlidePuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (inPuzzle) {
		GetInput();
	}
}


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
}

// Called when the game starts or when spawned
void ASlidePuzzle::BeginPlay()
{
	Super::BeginPlay();
	
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
	FVector Location = FVector();
	FTransform SpawnTransform = FTransform();

	SpawnTransform.SetScale3D(FVector(1 / PuzzleSize));
	SpawnTransform.SetLocation(Pivot->GetComponentLocation());
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	AActor* TempPipe = GetWorld()->SpawnActor<AActor>(PuzzlePiece, SpawnTransform);
	TempPipe->SetActorScale3D(GetActorScale3D() / PuzzleSize);

	TempPipe->GetActorBounds(true, Origin, BoxExtent, false);
	PieceSpacing = BoxExtent.Z * 2;

	Location = TempPipe->GetActorLocation();
	// Rotating the location vector to disregard the actor's initial rotation
	Location = Location.RotateAngleAxis(GetActorRotation().Yaw * -1, FVector(0, 0, 1));
	// Offsetting puzzle piece to row 0 column 0
	Location.X -= PieceSpacing * (PuzzleSize - 1) / 2;
	Location.Z += PieceSpacing * (PuzzleSize - 1) / 2;
	// Reverse the rotation
	Location = Location.RotateAngleAxis(GetActorRotation().Yaw, FVector(0, 0, 1));
	startPieceLocation = Location;

	// Removing temp puzzle piece after location and piece spacing are calculated
	TempPipe->Destroy();

	for (int row = 0; row < PuzzleSize; row++) {
		for (int col = 0; col < PuzzleSize; col++) {
			float pieceX = startPieceLocation.X + (row * PieceSpacing);
			float pieceZ = startPieceLocation.Z + (col * PieceSpacing);

			PuzzlePieces[row][col] = GetWorld()->SpawnActor<AActor>(PuzzlePiece, FVector(pieceX, Pivot->GetComponentLocation().Y, pieceZ), GetActorRotation());
			SetPieceTexture(row, col);
		}
	}
}

void ASlidePuzzle::SetPieceTexture(int row, int col)
{
	PuzzleImage = PuzzleImageArray[FMath::RandRange(0, PuzzleImageArray.Num() - 1)];

	if (PuzzlePieces[row][col]) {
		UStaticMeshComponent* PieceMesh;
		TArray<UActorComponent*> FoundComponents;
		AActor* currentPiece = PuzzlePieces[row][col];

		currentPiece->GetComponents(UStaticMeshComponent::StaticClass(), FoundComponents);
		PieceMesh = Cast<UStaticMeshComponent>(FoundComponents[0]);

		UMaterialInterface* tempMat = PieceMesh->GetMaterial(0);
		PuzzleMat = UMaterialInstanceDynamic::Create(tempMat, this);
		PieceMesh->SetMaterial(0, PuzzleMat);

		float fSize = PuzzleSize;
		float fRow = row;
		float fCol = col;
	
		PuzzleMat->GetTextureParameterValue(TEXT("SlideImage"), PuzzleImage);
		PuzzleMat->GetScalarParameterValue(TEXT("Size"), fSize);
		PuzzleMat->GetScalarParameterValue(TEXT("CustomU"), fRow);
		PuzzleMat->GetScalarParameterValue(TEXT("CustomU"), fCol);
	}
}

// Called every frame
void ASlidePuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


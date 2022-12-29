// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PythonBridge.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"


// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = mesh;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{	
	Super::BeginPlay();

	getSetupInfo();

	spawnedPlayers = 0;

}

AActor* AGrid::SpawnActor(TSubclassOf<AActor> Actor, FVector Loc, FRotator Rot, FName CellTag, FName Path)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Loc);
	SpawnTransform.SetRotation(Rot.Quaternion());
	SpawnTransform.SetScale3D(GetActorScale3D());

	
	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(Actor, SpawnTransform);
	SpawnedActorRef->SetActorScale3D(GetActorScale3D());

	SpawnedActorRef->Tags.Add(CellTag);
	SpawnedActorRef->SetFolderPath(Path);

	// Up
	if (Rot == FRotator(0, 0, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("UP"));
	}

	// Down
	else if (Rot == FRotator(0, 180, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("DOWN"));
	}

	// Left
	else if (Rot == FRotator(0, -90, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("LEFT"));
	}

	// Right
	else if (Rot == FRotator(0, 90, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("RIGHT"));
	}

	return SpawnedActorRef;
}


AActor* AGrid::SpawnActor(TSubclassOf<AActor> Actor, FVector Loc, FRotator Rot, FVector Scale, FName CellTag, FName Path)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Loc);
	SpawnTransform.SetRotation(Rot.Quaternion());
	SpawnTransform.SetScale3D(Scale);

	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(Actor, SpawnTransform);
	SpawnedActorRef->Tags.Add(CellTag);
	SpawnedActorRef->SetFolderPath(Path);
	SpawnedActorRef->SetActorScale3D(Scale);

	//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("test %s"), *(SpawnedActorRef->GetActorScale3D().ToString())), false);
	//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Transform %s"), *(SpawnTransform.GetScale3D().ToString())), false);


	// Adding rotation tag
	// Up
	if (Rot == FRotator(0, 0, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("UP"));
	}

	// Down
	else if (Rot == FRotator(0, 180, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("DOWN"));
	}

	// Left
	else if (Rot == FRotator(0, -90, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("LEFT"));
	}

	// Right
	else if (Rot == FRotator(0, 90, 0)) {
		SpawnedActorRef->Tags.Add(TEXT("RIGHT"));
	}

	return SpawnedActorRef;
}


void AGrid::GenerateFloorAndCeiling()
{
	//Cell floor
	FVector CellScale = GetActorScale3D();

	CellScale.X *= Rows;
	CellScale.Y *= Columns;

	FRotator CellRot = GetActorRotation();

	FVector CellLocation = GetActorLocation();
	CellLocation.Y += Rows / 2 * 1000 * GetActorScale3D().Y;
	CellLocation.X += Columns / 2 * 1000 * GetActorScale3D().X;
	CellLocation.Z = GetActorLocation().Z;


	if (Rows % 2 == 0) {
		CellLocation.Y -= 500 * GetActorScale3D().Y;
	}

	if (Columns % 2 == 0) {
		CellLocation.X -= 500 * GetActorScale3D().X;
	}

	SpawnActor(CellActor, CellLocation, CellRot, CellScale, FName(TEXT("CellFloor")), TEXT("Maze"));

	//Ceiling
	TArray<FString> CellWalls;

	CellLocation = GetActorLocation();
	CellLocation.Z += 480 * GetActorScale3D().Z;

	CellScale = GetActorScale3D();
	CellScale.Z = CellScale.X;

	for (int row = 0; row < Rows; row++)
	{
		CellLocation.X = GetActorLocation().X;
		for (int col = 0; col < Columns; col++)
		{
			CellWalls = GetCellWalls(row, col);

			// X / Cross
			if (CellWalls.Num() == 0)
			{
				SpawnActor(XActor, CellLocation, FRotator(0, 0, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan, FString("0 Walls"));
			}
			// T
			else if (CellWalls.Num() == 1)
			{
				if (CellWalls.Contains(FString("UP")))
				{
					SpawnActor(TActor, CellLocation, FRotator(0, 0, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("RIGHT")))
				{
					SpawnActor(TActor, CellLocation, FRotator(0, 90, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("DOWN")))
				{
					SpawnActor(TActor, CellLocation, FRotator(0, 180, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("LEFT")))
				{
					SpawnActor(TActor, CellLocation, FRotator(0, 270, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan, FString("1 Walls"));
			}
			// L or I
			else if (CellWalls.Num() == 2)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan, FString("2 Walls"));

				// L
				if (CellWalls.Contains(FString("LEFT")) && CellWalls.Contains(FString(TEXT("DOWN"))))
				{
					SpawnActor(LActor, CellLocation, FRotator(0, 0, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("UP")) && CellWalls.Contains(FString(TEXT("LEFT"))))
				{
					SpawnActor(LActor, CellLocation, FRotator(0, 90, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("RIGHT")) && CellWalls.Contains(FString(TEXT("UP"))))
				{
					SpawnActor(LActor, CellLocation, FRotator(0, 180, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("DOWN")) && CellWalls.Contains(FString(TEXT("RIGHT"))))
				{
					SpawnActor(LActor, CellLocation, FRotator(0, 270, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}

				// I

				else if (CellWalls.Contains(FString("LEFT")) && CellWalls.Contains(FString(TEXT("RIGHT"))))
				{
					SpawnActor(IActor, CellLocation, FRotator(0, 0, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("UP")) && CellWalls.Contains(FString(TEXT("DOWN"))))
				{
					SpawnActor(IActor, CellLocation, FRotator(0, 90, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
			}
			// U / One Way
			else if (CellWalls.Num() == 3)
			{
				if (CellWalls.Contains(FString("UP")) == false)
				{
					SpawnActor(UActor, CellLocation, FRotator(0, 0, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("RIGHT")) == false)
				{
					SpawnActor(UActor, CellLocation, FRotator(0, 90, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("DOWN")) == false)
				{
					SpawnActor(UActor, CellLocation, FRotator(0, 180, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
				else if (CellWalls.Contains(FString("LEFT")) == false)
				{
					SpawnActor(UActor, CellLocation, FRotator(0, 270, 0), CellScale, *FString::Printf(TEXT("Ceil (%d, %d)"), row, col), TEXT("Maze/Ceiling"));
				}
			}

			CellLocation.X += 1000 * GetActorScale3D().X;
		}
		CellLocation.Y += 1000 * GetActorScale3D().Y;
	}

	//CellLocation.Z += 480 * CellScale.Z;
	//SpawnActor(CeilingActor, CellLocation, CellRot, CellScale, FName(TEXT("Ceiling")), TEXT("Maze"));
}

void AGrid::OffsetWallTexture(AActor* wallActor, int wallCount)
{
	UStaticMeshComponent* wallMesh;
	TArray<UActorComponent*> foundComponents;
	UMaterialInterface* wallMat;
	UMaterialInstanceDynamic* wallMatDynamic;

	wallActor->GetComponents(UStaticMeshComponent::StaticClass(), foundComponents);
	wallMesh = Cast<UStaticMeshComponent>(foundComponents[0]);
	wallMat = wallMesh->GetMaterial(0);
	wallMatDynamic = UMaterialInstanceDynamic::Create(wallMat, wallActor);
	wallMesh->SetMaterial(0, wallMatDynamic);

	wallMatDynamic->SetScalarParameterValue(FName(TEXT("PixelOffset")), (0.1/Rows) * wallCount);
}


void AGrid::GenerateWalls()
{
	FVector Scale = GetActorScale3D();
	FVector CellLocation = GetActorLocation();

	FVector HorizWallLocation = CellLocation;
	HorizWallLocation.Y -= 500 * Scale.Y;

	FVector VertWallLocation = CellLocation;

	FVector PillarLocation = CellLocation;
	PillarLocation.Y -= 500 * Scale.Y;

	FRotator Rot = GetActorRotation();
	FRotator VertRot = Rot;
	VertRot.Yaw += 90;

	int totalWallsSpawned = 0;
	AActor* currentWall;

	//Main Walls
	for (int32 row = 0; row < Rows; ++row) {
		//CellLocation.X = GetActorLocation().X;

		HorizWallLocation.X = CellLocation.X;
		VertWallLocation.X = CellLocation.X - 500 * Scale.X;
		PillarLocation.X = CellLocation.X - 500 * Scale.X;
		for (int32 col = 0; col < Columns; ++col) {
			//FString CellString = FString::Printf(TEXT("Cell (%d, %d)"), row, col);
			//SpawnActor(CellActor, CellLocation, Rot, FName(*CellString), TEXT("Maze/Cells"));
			//CellLocation.X += 1000 * Scale.X;
		
			FString HorizWallString = FString::Printf(TEXT("(%d, %d)|(%d, %d)"), row-1, col, row, col);
			currentWall = SpawnActor(WallActor, HorizWallLocation, Rot, FName(*HorizWallString), TEXT("Maze/Walls"));
			HorizWallLocation.X += 1000 * Scale.X;
			OffsetWallTexture(currentWall, totalWallsSpawned++);

			FString VertWallString = FString::Printf(TEXT("(%d, %d)|(%d, %d)"), row, col-1, row, col);
			currentWall = SpawnActor(WallActor, VertWallLocation, VertRot, FName(*VertWallString), TEXT("Maze/Walls"));
			VertWallLocation.X += 1000 * Scale.X;
			OffsetWallTexture(currentWall, totalWallsSpawned++);

			FString PillarString = FString::Printf(TEXT("Pillar (%d, %d)"), row, col);
			SpawnActor(PillarActor, PillarLocation, Rot, FName(*PillarString), TEXT("Maze/Pillars"));
			PillarLocation.X += 1000 * Scale.X;
		}
		//CellLocation.Y += 1000 * Scale.Y;
		HorizWallLocation.Y += 1000 * Scale.Y;
		VertWallLocation.Y += 1000 * Scale.Y;
		PillarLocation.Y += 1000 * Scale.Y;
	}

	//Last Horiz Walls and Pillars
	HorizWallLocation.X = GetActorLocation().X;
	PillarLocation.X = GetActorLocation().X - 500 * Scale.X;
	for (int32 col = 0; col < Columns; ++col) {
		FString HorizWallString = FString::Printf(TEXT("(%d, %d)|(%d, %d)"), Rows-1, col, Rows, col);
		currentWall = SpawnActor(WallActor, HorizWallLocation, Rot, FName(*HorizWallString), TEXT("Maze/Walls"));
		HorizWallLocation.X += 1000 * Scale.X;
		OffsetWallTexture(currentWall, totalWallsSpawned++);

		FString PillarString = FString::Printf(TEXT("Pillar (%d, %d)"), Rows, col);
		SpawnActor(PillarActor, PillarLocation, Rot, FName(*PillarString), TEXT("Maze/Pillars"));
		PillarLocation.X += 1000 * Scale.X;
	}

	//Last Vert Walls and Pillars
	VertWallLocation.Y = GetActorLocation().Y;
	PillarLocation.Y = GetActorLocation().Y - 500 * Scale.Y;
	for (int32 row = 0; row < Columns; ++row) {
		FString VertWallString = FString::Printf(TEXT("(%d, %d)|(%d, %d)"), row, Columns-1, row, Columns);
		currentWall = SpawnActor(WallActor, VertWallLocation, VertRot, FName(*VertWallString), TEXT("Maze/Walls"));
		VertWallLocation.Y += 1000 * Scale.Y;
		OffsetWallTexture(currentWall, totalWallsSpawned++);

		FString PillarString = FString::Printf(TEXT("Pillar (%d, %d)"), row, Columns);
		SpawnActor(PillarActor, PillarLocation, Rot, FName(*PillarString), TEXT("Maze/Pillars"));
		PillarLocation.Y += 1000 * Scale.Y;
	}

	FString PillarString = FString::Printf(TEXT("Pillar (%d, %d)"), Rows, Columns);
	SpawnActor(PillarActor, PillarLocation, Rot, FName(*PillarString), TEXT("Maze/Pillars"));

}

TArray<FString> AGrid::GetCellWalls(int Row, int Column)
{
	TArray<FString> Directions;

	TArray<AActor*> FoundWallActors;

	TArray<AActor*> ActorsFoundInCell;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *FString::Printf(TEXT("(%d, %d)"), Row, Column), ActorsFoundInCell);

	if (ActorsFoundInCell.Num() == 0) {
		// Up
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), WallActor, *FString::Printf(TEXT("(%d, %d)|(%d, %d)"), Row - 1, Column, Row, Column), FoundWallActors);
		if (FoundWallActors.Num() > 0) {
			Directions.Add(FString(TEXT("UP")));
		}

		// Down
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), WallActor, *FString::Printf(TEXT("(%d, %d)|(%d, %d)"), Row, Column, Row + 1, Column), FoundWallActors);
		if (FoundWallActors.Num() > 0) {
			Directions.Add(FString(TEXT("DOWN")));
		}

		// Left
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), WallActor, *FString::Printf(TEXT("(%d, %d)|(%d, %d)"), Row, Column - 1, Row, Column), FoundWallActors);
		if (FoundWallActors.Num() > 0) {
			Directions.Add(FString(TEXT("LEFT")));
		}

		// Right
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), WallActor, *FString::Printf(TEXT("(%d, %d)|(%d, %d)"), Row, Column, Row, Column + 1), FoundWallActors);
		if (FoundWallActors.Num() > 0) {
			Directions.Add(FString(TEXT("RIGHT")));
		}
	}

	return Directions;
}


void AGrid::GenerateLockers()
{
	float RandomUpperBound;

	if (LockerSpawningPercent > 0)
	{
		RandomUpperBound = 1 / LockerSpawningPercent;

		int32 RandomChance;
		int32 RandomIndex;
		TArray<FString> Directions;

		FVector Scale = GetActorScale3D();
		Scale.Z = Scale.X;

		for (int32 row = 0; row < Rows; ++row) {
			for (int32 col = 0; col < Columns; ++col) {

				RandomChance = UKismetMathLibrary::RandomIntegerInRange(1, (int32)RandomUpperBound);
				if (RandomChance == 1 && !(row == 0 && col == Columns / 2)) {
					FVector Location = GetActorLocation();
					Location.X += 1000 * col * Scale.X;
					Location.Y += 1000 * row * Scale.Y;

					Directions = GetCellWalls(row, col);

					RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, Directions.Num() - 1);

					if (Directions.Num() > 0) {
						// Up
						if (FString(TEXT("UP")).Equals(Directions[RandomIndex])) {
							SpawnActor(LockerActor[FMath::RandRange(0, (LockerActor.Num() - 1))], Location, FRotator(0, 0, 0), Scale, *FString::Printf(TEXT("(%d, %d)"), row, col), FName(TEXT("Maze/Lockers")));
						}

						// Down
						else if (FString(TEXT("DOWN")).Equals(Directions[RandomIndex])) {
							SpawnActor(LockerActor[FMath::RandRange(0, (LockerActor.Num() - 1))], Location, FRotator(0, 180, 0), Scale, *FString::Printf(TEXT("(%d, %d)"), row, col), FName(TEXT("Maze/Lockers")));
						}

						// Left
						else if (FString(TEXT("LEFT")).Equals(Directions[RandomIndex])) {
							SpawnActor(LockerActor[FMath::RandRange(0, (LockerActor.Num() - 1))], Location, FRotator(0, -90, 0), Scale, *FString::Printf(TEXT("(%d, %d)"), row, col), FName(TEXT("Maze/Lockers")));
						}

						// Right
						else if (FString(TEXT("RIGHT")).Equals(Directions[RandomIndex])) {
							SpawnActor(LockerActor[FMath::RandRange(0, (LockerActor.Num() - 1))], Location, FRotator(0, 90, 0), Scale, *FString::Printf(TEXT("(%d, %d)"), row, col), FName(TEXT("Maze/Lockers")));
						}
					}
				}
			}
		}
	}	
}


void AGrid::GeneratePuzzles()
{
	// Adding Puzzles To Array
	TArray<TSubclassOf<AActor>> PuzzleActors = PuzzleActorArray;

	//PuzzleActors.Add(LockPuzzleActor);
	//PuzzleActors.Add(KeyFrameActor);
	

	TArray<AActor*> ActorsFoundInCell;
	int32 totalCells = Rows * Columns;

	FVector Scale = GetActorScale3D();
	Scale.Z = Scale.X;

	FVector Location;

	TArray<FString> directions;
	int32 randomDirectionIndex;

	int32 Row, Column;

	FRotator rotation;

	TArray<FCell> Cells;

	// Creating array of cell types
	for (int32 row = 0; row < Rows; ++row) {
		for (int32 col = 0; col < Columns; ++col) {
			if (!(row == 0 && col == Columns / 2))
			{
				Cells.Add(FCell(row, col));
			}
		}
	}

	// Shuffles array for some reason
	Cells.Sort([this](const FCell Item1, const FCell Item2) {
		return FMath::FRand() < 0.5f;
	});


	while (PuzzleActors.Num() > 0 && Cells.Num() > 0) {
		Row = Cells[0][0];
		Column = Cells[0][1];

		directions = GetCellWalls(Row, Column);
		randomDirectionIndex = FMath::RandRange(0, directions.Num() - 1);

		Location = GetActorLocation();
		Location.X += 1000 * Column * Scale.X;
		Location.Y += 1000 * Row * Scale.Y;

		if (directions.Num() > 0) {
			// Up
			if (FString(TEXT("UP")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 0, 0);
			}

			// Down
			else if (FString(TEXT("DOWN")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 180, 0);
			}

			// Left
			else if (FString(TEXT("LEFT")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, -90, 0);
			}

			// Right
			else if (FString(TEXT("RIGHT")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 90, 0);
			}

			SpawnActor(PuzzleActors[0], Location, rotation, Scale, *FString::Printf(TEXT("(%d, %d)"), Row, Column), FName(TEXT("Maze/Puzzle")));
			PuzzleActors.RemoveAt(0);
			UE_LOG(LogTemp, Warning, TEXT("Puzzle %s"), *(Cells[0].ToString()))
		}
		Cells.RemoveAt(0);
	}
}


void AGrid::GenerateProps()
{
	// Adding Puzzles To Array
	TArray<TSubclassOf<AActor>> PropActors = PropActorArray;

	//PropActors.Add(LockPropActor);
	//PropActors.Add(KeyFrameActor);


	TArray<AActor*> ActorsFoundInCell;
	int32 totalCells = Rows * Columns;

	FVector Scale = GetActorScale3D();
	Scale.Z = Scale.X;

	FVector Location;

	TArray<FString> directions;
	int32 randomDirectionIndex, randomPropIndex;

	int32 Row, Column;

	int32 NumProps, NumPropsToSpawn;

	FRotator rotation;

	TArray<FCell> Cells;

	// Creating array of cell types

	TArray<AActor*> FoundActors;

	for (int32 row = 0; row < Rows; ++row) {
		for (int32 col = 0; col < Columns; ++col) {
			if (!(row == 0 && col == Columns / 2))
			{
				FCell TempCell = FCell(row, col);
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), *TempCell.ToString(), FoundActors);

				if (FoundActors.Num() == 0)
				{
					Cells.Add(TempCell);
					//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan, FString::Printf(TEXT("Empty Cell %s"), *TempCell.ToString()));
				}
			}
		}
	}

	// Shuffles array for some reason
	Cells.Sort([this](const FCell Item1, const FCell Item2) {
		return FMath::FRand() < 0.5f;
	});


	NumProps = PropActors.Num();
	NumPropsToSpawn = Cells.Num() * PropSpawningPercent;


	for (int i = 0; i < NumPropsToSpawn; ++i)
	{
		Row = Cells[i][0];
		Column = Cells[i][1];

		directions = GetCellWalls(Row, Column);
		randomDirectionIndex = FMath::RandRange(0, directions.Num() - 1);
		randomPropIndex = FMath::RandRange(0, PropActors.Num() - 1);

		Location = GetActorLocation();
		Location.Y += 1000 * Row * Scale.Y;
		Location.X += 1000 * Column * Scale.X;

		if (directions.Num() > 0) {
			// Up
			if (FString(TEXT("UP")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 0, 0);
			}

			// Down
			else if (FString(TEXT("DOWN")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 180, 0);
			}

			// Left
			else if (FString(TEXT("LEFT")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, -90, 0);
			}

			// Right
			else if (FString(TEXT("RIGHT")).Equals(directions[randomDirectionIndex])) {
				rotation = FRotator(0, 90, 0);
			}

			SpawnActor(PropActors[randomPropIndex], Location, rotation, Scale, *FString::Printf(TEXT("(%d, %d)"), Row, Column), FName(TEXT("Maze/Prop")));
			UE_LOG(LogTemp, Warning, TEXT("Prop %s"), *Cells[i].ToString());

		}
	}
}


void AGrid::SetupMazeAlgorithmComponent()
{
	if (MazeAlgorithmSubclass) {
		MazeAlgorithm = NewObject<UMazeAlgorithm>(this, MazeAlgorithmSubclass);
		MazeAlgorithm->SetDimensions(Rows, Columns);
	}
}

void AGrid::SetupToastWalkableSurface()
{
	AActor* ToastWalkableSurfaceRef = UGameplayStatics::GetActorOfClass(GetWorld(), NavMeshBoundsVolume);
	if (ToastWalkableSurfaceRef != NULL) {
		ToastWalkableSurface = Cast<AVolume>(ToastWalkableSurfaceRef);

		//Location
		FVector newLocation = GetActorLocation();

		newLocation.Y += Rows / 2 * 1000 * GetActorScale3D().Y;
		newLocation.X += Columns / 2 * 1000 * GetActorScale3D().X;
		newLocation.Z = GetActorLocation().Z;

		//Accounting for even rows / columns
		if (Rows % 2 == 0) {
			newLocation.Y -= 500 * GetActorScale3D().Y;
		}

		if (Columns % 2 == 0) {
			newLocation.X -= 500 * GetActorScale3D().X;
		}

		ToastWalkableSurface->SetActorLocation(newLocation);



		//Scale
		FVector newScale = GetActorScale3D();
		newScale.Z = 50;

		newScale.X *= 5 * Rows;
		newScale.Y *= 5 * Columns;

		ToastWalkableSurface->SetActorScale3D(newScale);

		
		if (UNavigationSystemV1* navigation_system = UNavigationSystemV1::GetCurrent(GetWorld())) {
			navigation_system->Build();
		}
	}
	
}


void AGrid::GenerateDoorKeyhole()
{
	if (DoorKeyholeActor)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.X += 1000 * GetActorScale3D().X * (Columns / 2);

		SpawnActor(DoorKeyholeActor, SpawnLocation, FRotator(0, 0, 0), FName(TEXT("DoorKeyhole")), FName(TEXT("Maze/Puzzle")));
	}
}

void AGrid::getSetupInfo()
{

	UGISetup* GameInstance = Cast<UGISetup>(GetWorld()->GetGameInstance());

	if (GameInstance->TestingGame == false) {

		if (GameInstance->Map == 1) {
			PropActorArray.Empty();

			for (int i = 0; i < BackroomPropsArray.Num(); i++) {
				PropActorArray.Add(BackroomPropsArray[i]);
			}
		}
		else if (GameInstance->Map == 2) {
			PropActorArray.Empty();

			for (int i = 0; i < BackroomPropsArray.Num(); i++) {
				PropActorArray.Add(HotelPropsArray[i]);
			}
		}

		if (GameInstance->Map != 0) {
			LActor = GameInstance->L;
			IActor = GameInstance->I;
			UActor = GameInstance->U;
			XActor = GameInstance->X;
			TActor = GameInstance->T;
		}

		Rows = GameInstance->GridSize;
		Columns = GameInstance->GridSize;

		//Still shuffles for some reason?
		PuzzleActorArray.Sort([this](const TSubclassOf<AActor> Actor1, const TSubclassOf<AActor> Actor2) {
			return FMath::FRand() < 0.5f;
			});

		for (int i = 0; i < GameInstance->PuzzleAmount; i++) {

			puzzleArrayClone.Add(PuzzleActorArray[i]);

		}

		PuzzleActorArray = puzzleArrayClone;

		LockerSpawningPercent = GameInstance->HidingSpawnRate;

	}

	beginGeneration();

}

void AGrid::beginGeneration()
{

	SetupMazeAlgorithmComponent();
	GenerateWalls();

	if (MazeAlgorithm) {
		MazeAlgorithm->GenerateRemovedWalls(WallActor);
	}

	GenerateFloorAndCeiling();

	GenerateDoorKeyhole();

	GeneratePuzzles();

	GenerateLockers();

	GenerateProps();

	

	SetupToastWalkableSurface();

	if (bSpawnToast)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 20;
		GetWorld()->SpawnActor<AActor>(ToastActor, SpawnLocation, FRotator(0, 0, 0));
	}

}


void AGrid::RemoveWalls()
{ 
	/*FString Command = FString::Printf(TEXT("ExecutePythonScript C:\\Users\\TysonLoveJr\\Desktop\\JR\\Coding\\BrentToast\\Game\\TheBrentCave\\Python\\main.py %d %d"), Rows, Columns);
	GEngine->Exec(NULL, *Command);*/
	/*TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTa g(GetWorld(), CellActor, FName("Hello"), FoundActors);*/

	//FoundActors[1]->Destroy();
	 
	/*UPythonBridge* bridge = UPythonBridge::Get();
	if (bridge)
	{
		bridge->RemoveWalls(Rows, Columns, WallActor, GetWorld());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Cyan, FString("Error In Bridge"));
	}*/
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


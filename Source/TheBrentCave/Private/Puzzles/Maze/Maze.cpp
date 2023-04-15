// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Maze/Maze.h"

// Sets default values
AMaze::AMaze()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PuzzleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Puzzle Mesh"));
	PuzzleMesh->SetupAttachment(RootComponent);

	MazeWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	MazeWidgetComponent->SetupAttachment(PuzzleMesh);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Component"));
	InteractComponent->SetupAttachment(RootComponent);

	MazeCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Maze Camera"));
	MazeCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	bIsAnyAnimationPlaying = false;

}


// Called when the game starts or when spawned
void AMaze::BeginPlay()
{
	Super::BeginPlay();

	PuzzleLight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

	bInPuzzle = false;

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/InteractionWidget.InteractionWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {

		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = InteractComponent->GetWidget();
	}


	if (MazeWidgetClass && Dimensions)
	{
		MazeWidgetComponent->SetWidgetClass(MazeWidgetClass);
		MazeWidget = Cast<UMazePuzzleWidget>(MazeWidgetComponent->GetWidget());
		MazeWidget->SetMazeAlgorithm(MazeAlgorithmClass);
		MazeWidget->HighlightedCellClass = HighlightedCellClass;
		MazeWidget->WallClass = WallClass;
		MazeWidget->SetupWidget(Dimensions, Dimensions, MazeWidgetComponent->GetDrawSize().Y - MazeWidgetComponent->GetDrawSize().Y / Dimensions / 4);
		MazeWidgetComponent->SetVisibility(false);
		MazeWidgetComponent->SetWindowVisibility(EWindowVisibility::SelfHitTestInvisible);

		SetupInputComponent();

		CurrentCell = FCell(Dimensions / 2, Dimensions / 2);

		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMaze::OnOverlapStart);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AMaze::OnOverlapEnd);
	}
}


void AMaze::SetupInputComponent()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	InputComponent->BindAction("Interact", IE_Pressed, this, &AMaze::InteractPuzzle);
	DisableInput(GetWorld()->GetFirstPlayerController());
}


void AMaze::ToggleMovementKeyBinds()
{
	if (bInPuzzle)
	{
		InputComponent->BindKey(EKeys::Left, IE_Pressed, this, &AMaze::MoveLeft);
		InputComponent->BindKey(EKeys::Right, IE_Pressed, this, &AMaze::MoveRight);
		InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AMaze::MoveUp);
		InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AMaze::MoveDown);
	}
	else
	{
		for (int i = 0; i < InputComponent->KeyBindings.Num(); i++)
		{
			if (InputComponent->KeyBindings[i].Chord == FInputChord(EKeys::Left))
			{
				InputComponent->KeyBindings.RemoveAt(i);
			}
			else if (InputComponent->KeyBindings[i].Chord == FInputChord(EKeys::Right))
			{
				InputComponent->KeyBindings.RemoveAt(i);
			}
			else if (InputComponent->KeyBindings[i].Chord == FInputChord(EKeys::Up))
			{
				InputComponent->KeyBindings.RemoveAt(i);
			}
			else if (InputComponent->KeyBindings[i].Chord == FInputChord(EKeys::Down))
			{
				InputComponent->KeyBindings.RemoveAt(i);
			}
		}
	}
}


void AMaze::OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HitBoxPlayer = Cast<ATBCCharacter>(OtherActor);
	if (HitBoxPlayer) {
		EnableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(true);

		if (HitBoxPlayer->Tags.Num() > 0)
		{

			FName PlayerTag = HitBoxPlayer->Tags[0];
			TArray<AActor*> FoundActors;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), PlayerTag, FoundActors);

			if (FoundActors.Num() > 0)
			{
				HitBoxPlayer = Cast<ATBCCharacter>(FoundActors[0]);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
			}
		}
	}
}


void AMaze::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
	}
}


void AMaze::EnterPuzzle()
{
	if (!bInPuzzle && HitBoxPlayer)
	{
		bInPuzzle = true;

		GetWorldTimerManager().ClearAllTimersForObject(this);

		InteractComponent->SetVisibility(false);

		HitBoxPlayer->SetActorEnableCollision(false);

		PuzzleLight->SetVisibility(true);
		MazeWidgetComponent->SetVisibility(true);
		MazeWidgetComponent->SetWindowVisibility(EWindowVisibility::Visible);
		HitBoxPlayer->SetActorHiddenInGame(true);

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMaze::ToggleMovementKeyBinds, 0.6f, false);

		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}


void AMaze::LeavePuzzle()
{
	if (bInPuzzle && HitBoxPlayer)
	{
		bInPuzzle = false;

		GetWorldTimerManager().ClearAllTimersForObject(this);

		InteractComponent->SetVisibility(true);

		HitBoxPlayer->SetActorEnableCollision(true);

		PuzzleLight->SetVisibility(false);
		MazeWidgetComponent->SetVisibility(false);
		HitBoxPlayer->SetActorHiddenInGame(false);

		ToggleMovementKeyBinds();

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);

	}
}


void AMaze::InteractPuzzle()
{
	if (bInPuzzle) {
		LeavePuzzle();
	}
	else {
		EnterPuzzle();
	}
}


void AMaze::MoveLeft()
{
	if (!bIsAnyAnimationPlaying)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Left")));
		FCell NextCell = FCell(CurrentCell[0], CurrentCell[1] - 1);

		// If the next cell column is greater than or equal to 0
		if (NextCell[1] >= 0)
		{
			FWall WallName = FWall(NextCell, CurrentCell);

			UWidget* WallToFind = MazeWidget->WidgetTree->FindWidget(*WallName.ToString());

			// If wall to find doesn't exist
			if (!WallToFind)
			{
				UWidget* NextCellCheck = MazeWidget->WidgetTree->FindWidget(*FString::Printf(TEXT("Highlighted %s"), *NextCell.ToString()));

				// If next cell is not already created
				if (!NextCellCheck)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("New Cell: %s"), *NextCell.ToString()));
					CurrentCell = NextCell;

					bIsAnyAnimationPlaying = true;
					MazeWidget->SummonHighlightedCell(CurrentCell, UHighlightedCellWidget::LEFT);

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("AnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
				// If the next cell has already been created
				else {
					bIsAnyAnimationPlaying = true;
					MazeWidget->ReverseHighlightedCell(CurrentCell);
					CurrentCell = NextCell;

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("ReverseAnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Wall In The Way")));
			}
		}
	}
}


void AMaze::MoveRight()
{
	if (!bIsAnyAnimationPlaying)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Right")));
		FCell NextCell = FCell(CurrentCell[0], CurrentCell[1] + 1);

		// If the next cell column is less than Dimensions
		if (NextCell[1] < Dimensions)
		{
			FWall WallName = FWall(NextCell, CurrentCell);

			UWidget* WallToFind = MazeWidget->WidgetTree->FindWidget(*WallName.ToString());

			// If wall to find doesn't exist
			if (!WallToFind)
			{
				UWidget* NextCellCheck = MazeWidget->WidgetTree->FindWidget(*FString::Printf(TEXT("Highlighted %s"), *NextCell.ToString()));

				// If next cell is not already created
				if (!NextCellCheck)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("New Cell: %s"), *NextCell.ToString()));
					CurrentCell = NextCell;

					bIsAnyAnimationPlaying = true;
					MazeWidget->SummonHighlightedCell(CurrentCell, UHighlightedCellWidget::RIGHT);

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("AnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
				// If the next cell has already been created
				else {
					bIsAnyAnimationPlaying = true;
					MazeWidget->ReverseHighlightedCell(CurrentCell);
					CurrentCell = NextCell;

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("ReverseAnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Wall In The Way")));
			}
		}
	}
}


void AMaze::MoveUp()
{
	if (!bIsAnyAnimationPlaying)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Up")));
		FCell NextCell = FCell(CurrentCell[0] - 1, CurrentCell[1]);

		// If the next cell row is greater than or equal to 0
		if (NextCell[0] >= 0)
		{
			FWall WallName = FWall(NextCell, CurrentCell);

			UWidget* WallToFind = MazeWidget->WidgetTree->FindWidget(*WallName.ToString());

			// If wall to find doesn't exist
			if (!WallToFind)
			{
				UWidget* NextCellCheck = MazeWidget->WidgetTree->FindWidget(*FString::Printf(TEXT("Highlighted %s"), *NextCell.ToString()));

				// If next cell is not already created
				if (!NextCellCheck)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("New Cell: %s"), *NextCell.ToString()));
					CurrentCell = NextCell;

					bIsAnyAnimationPlaying = true;
					MazeWidget->SummonHighlightedCell(CurrentCell, UHighlightedCellWidget::UP);

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("AnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
				// If the next cell has already been created
				else {
					bIsAnyAnimationPlaying = true;
					MazeWidget->ReverseHighlightedCell(CurrentCell);
					CurrentCell = NextCell;

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("ReverseAnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Wall In The Way")));
			}
		}
		else if (NextCell == FCell(-1, Dimensions / 2))
		{
			// This calls the win function

			LeavePuzzle();

			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMaze::WinPuzzle, 0.6, false);
		}
	}
}


void AMaze::MoveDown()
{
	if (!bIsAnyAnimationPlaying)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Down")));
		FCell NextCell = FCell(CurrentCell[0] + 1, CurrentCell[1]);

		// If the next cell row is less than Dimensions
		if (NextCell[0] < Dimensions)
		{
			FWall WallName = FWall(NextCell, CurrentCell);

			UWidget* WallToFind = MazeWidget->WidgetTree->FindWidget(*WallName.ToString());

			// If wall to find doesn't exist
			if (!WallToFind)
			{
				UWidget* NextCellCheck = MazeWidget->WidgetTree->FindWidget(*FString::Printf(TEXT("Highlighted %s"), *NextCell.ToString()));

				// If next cell is not already created
				if (!NextCellCheck)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("New Cell: %s"), *NextCell.ToString()));
					CurrentCell = NextCell;

					bIsAnyAnimationPlaying = true;
					MazeWidget->SummonHighlightedCell(CurrentCell, UHighlightedCellWidget::DOWN);

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("AnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
				// If the next cell has already been created
				else {
					bIsAnyAnimationPlaying = true;
					MazeWidget->ReverseHighlightedCell(CurrentCell);
					CurrentCell = NextCell;

					MazeWidget->CurrentHighlightedCell->UnbindAllFromAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim);
					FWidgetAnimationDynamicEvent EndAnimationEvent;
					EndAnimationEvent.BindUFunction(this, FName("ReverseAnimationDone"));
					MazeWidget->CurrentHighlightedCell->BindToAnimationFinished(MazeWidget->CurrentHighlightedCell->SpawnAnim, EndAnimationEvent);
				}
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Wall In The Way")));
			}
		}
	}
}


void AMaze::AnimationDone()
{
	bIsAnyAnimationPlaying = false;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Animation Done")));
}


void AMaze::ReverseAnimationDone()
{
	bIsAnyAnimationPlaying = false;

	if (MazeWidget->CurrentHighlightedCell)
	{
		MazeWidget->WidgetTree->RemoveWidget(MazeWidget->CurrentHighlightedCell);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, *FString::Printf(TEXT("Reverse Animation Done")));
	}
}


void AMaze::WinPuzzle()
{

	AActor* breakWordSearch = GetWorld()->SpawnActor<AActor>(MazeBreakActor, PuzzleMesh->GetComponentLocation(), GetActorRotation());
	Destroy();
}


// Called every frame
void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


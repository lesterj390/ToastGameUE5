// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/WordSearch/WordSearch.h"

// Sets default values
AWordSearch::AWordSearch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	puzzleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Puzzle Base"));
	puzzleMesh->SetupAttachment(Pivot);

	GridWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grid Widget"));
	GridWidgetComponent->SetupAttachment(Pivot);

	WordWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Word Widget"));
	WordWidgetComponent->SetupAttachment(Pivot);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(Pivot);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PuzzleCamera->SetupAttachment(Pivot);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(Pivot);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(Pivot);

	static ConstructorHelpers::FObjectFinder<USoundCue> wrongSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/pressedWrongSound.pressedWrongSound'"));
	if (wrongSoundObject.Succeeded())
	{
		wrongSound = wrongSoundObject.Object;

		wrongAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("wrongAudioComponent"));
		wrongAudioComponent->SetupAttachment(RootComponent);

	}

	selectedRow = 0;
	selectedCol = 0;
	selectedRotation = 1;
	winRotation = 0;

	inOverlap = false;
	inPuzzle = false;
	canMoveSelection = true;

	GridWidgetComponent->SetVisibility(false);
	WordWidgetComponent->SetVisibility(false);
	Spotlight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

}

// Called when the game starts or when spawned
void AWordSearch::BeginPlay()
{
	Super::BeginPlay();

	GridWidgetComponent->SetVisibility(false);
	WordWidgetComponent->SetVisibility(false);
	Spotlight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

	SetupInput();

	/*if (GridWidgetSubclass) {

		GridWidget = CreateWidget<UWordSearchWidget>(GetWorld(), GridWidgetSubclass);
		GridWidget->AddToViewport();
		GridWidget->SetupWidget();

	}*/

	FSoftClassPath widgetClassRef(TEXT("/Game/_Main/Puzzles/WordSearch/Blueprint/BP_WordSearchWidget.BP_WordSearchWidget_C"));

	if (UClass* widgetClass = widgetClassRef.TryLoadClass<UWordSearchWidget>()) {

		/*GridWidget = CreateWidget<UWordSearchWidget>(GetWorld(), widgetClass);
		GridWidget->AddToViewport();*/

		GridWidgetComponent->SetWidgetClass(widgetClass);
		GridWidget = Cast<UWordSearchWidget>(GridWidgetComponent->GetWidget());

		GenerateWordBank();
		GridWidget->SetupWidget(GridWidgetComponent->GetDrawSize().X, MinPuzzleSize, MaxPuzzleSize);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Yay!")));

	}

	FSoftClassPath wordClassRef(TEXT("/Game/_Main/Puzzles/WordSearch/Blueprint/SelectedWordWidget.SelectedWordWidget_C"));

	if (UClass* wordClass = wordClassRef.TryLoadClass<UUserWidget>()) {

		/*GridWidget = CreateWidget<UWordSearchWidget>(GetWorld(), widgetClass);
		GridWidget->AddToViewport();*/

		WordWidgetComponent->SetWidgetClass(wordClass);
		WordWidget = Cast<UUserWidget>(WordWidgetComponent->GetWidget());

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Yay!")));

	}

	SetWord();

	if (wrongAudioComponent && wrongSound) {
		wrongAudioComponent->SetSound(wrongSound);
	}

}

// Called every frame
void AWordSearch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (inPuzzle == true) {

		if (canMoveSelection == true) {

			if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up) && selectedRow > 0) {
				selectedRow--;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X, GridWidget->imageSlot->GetPosition().Y + (GridWidget->spacing * -1)));
			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right) && selectedCol < GridWidget->PuzzleSize - 1) {
				selectedCol++;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X + GridWidget->spacing, GridWidget->imageSlot->GetPosition().Y));
			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down) && selectedRow < GridWidget->PuzzleSize - 1) {
				selectedRow++;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X, GridWidget->imageSlot->GetPosition().Y + GridWidget->spacing));
			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left) && selectedCol > 0) {
				selectedCol--;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X + (GridWidget->spacing * -1), GridWidget->imageSlot->GetPosition().Y));
			}

		}
		else {

			if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up) && selectedRotation > 0) {
				
				selectedRotation--;
				GridWidget->newRotation = selectedRotation;
				GridWidget->CheckRotation();

			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down) && selectedRotation < 3) {
				
				selectedRotation++;
				GridWidget->newRotation = selectedRotation;
				GridWidget->CheckRotation();

			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left) && selectedCol > 0) {
				canMoveSelection = true;
				GridWidget->ClearImageArray();
				selectedCol--;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X + (GridWidget->spacing * -1), GridWidget->imageSlot->GetPosition().Y));
			}
			else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right) && selectedCol < GridWidget->PuzzleSize - 1) {
				canMoveSelection = true;
				GridWidget->ClearImageArray();
				selectedCol++;
				GridWidget->imageSlot->SetPosition(FVector2D(GridWidget->imageSlot->GetPosition().X + GridWidget->spacing, GridWidget->imageSlot->GetPosition().Y));
			}
			
			//Rotations: 0 Straight, 1 Up Diagonal, 2 Down Diagonal, 3 Down

			if (selectedRotation == 0) {
				winRotation = 1;
			}
			else if (selectedRotation == 1) {
				winRotation = 0;
			}
			else if (selectedRotation == 2) {
				winRotation = 2;
			}
			else if (selectedRotation == 3) {
				winRotation = 3;
			}

		}

	}

}

void AWordSearch::EnterPuzzle()
{

	if (inOverlap == true && inPuzzle == false) {
		if (HitBoxPlayer)
		{

			inPuzzle = true;

			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
			Spotlight->SetVisibility(true);

			HitBoxPlayer->SetActorHiddenInGame(true);
			GridWidgetComponent->SetVisibility(true);
			WordWidgetComponent->SetVisibility(true);
			InteractComponent->SetVisibility(false);
			//InteractComponent->SetVisibility(false);

		}

	}

}

void AWordSearch::ExitPuzzle()
{

	if (inOverlap == true && inPuzzle == true) {

		canMoveSelection = true;
		GridWidget->ClearImageArray();

		if (HitBoxPlayer)
		{
			inPuzzle = false;

			Spotlight->SetVisibility(false);
			HitBoxPlayer->SetActorHiddenInGame(false);
			GridWidgetComponent->SetVisibility(false);
			WordWidgetComponent->SetVisibility(false);
			InteractComponent->SetVisibility(true);

			//InteractComponent->SetVisibility(true);


			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);
		}

	}

}


void AWordSearch::Select()
{
	if (inPuzzle == true) {
		if (canMoveSelection == true) {
			canMoveSelection = false;
			selectedRotation = 1;
			winRotation = 0;

			GridWidget->CheckRotation();
		}
		else {
			if (selectedRow == GridWidget->RowSpawn && selectedCol == GridWidget->ColSpawn && winRotation == GridWidget->WordRotation) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Win!")));
				WinPuzzle();
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Wrong!")));

				if (wrongAudioComponent && wrongSound) {

					wrongAudioComponent->Play(0.f);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Played Audio")));

				}
			}
		}

	}
}


void AWordSearch::SetupInput()
{

	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AWordSearch::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &AWordSearch::OnOverlapEnd);

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &AWordSearch::InteractPuzzle);
	InputComponent->BindAction("Use", IE_Pressed, this, &AWordSearch::Select);
	DisableInput(GetWorld()->GetFirstPlayerController());

}

void AWordSearch::InteractPuzzle()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Interacted")));

	if (inPuzzle == true) {
		ExitPuzzle();
	}
	else {
		EnterPuzzle();
	}

}

void AWordSearch::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AWordSearch::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Exited Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//HitBoxPlayer->InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AWordSearch::GenerateWordBank()
{

	FString dir = FPaths::ProjectContentDir();
	FString myFile = dir + "/_Main/Puzzles/WordSearch/File/wordlibrary.txt";
	//IFileManager::Get().FindFiles(WordBank, *dir, *fileExt);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Directory: %s"), *dir));


	if (FPaths::FileExists(myFile)) {

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Found File!")));
		FFileHelper::LoadFileToStringArray(WordBankArray, *myFile);

		if (WordBankArray.Num() > 0) {

			ChosenWord = WordBankArray[FMath::RandRange(0, WordBankArray.Num() - 1)];

			ChosenWord = ChosenWord.ToUpper();

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%s"), *ChosenWord));

			GridWidget->SetWord(ChosenWord);

		}
	}
}		

void AWordSearch::SetWord()
{

	UTextBlock* TextControl = (UTextBlock*)(WordWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("WordToFind")))));
	FString Number = FString::Printf(TEXT("%s"), *ChosenWord);
	TextControl->SetText(FText::FromString("Word to Find: " + Number));

}

void AWordSearch::WinPuzzle()
{

	ExitPuzzle();
	GetWorldTimerManager().SetTimer(breakPuzzle, this, &AWordSearch::DestroyPuzzle, 0.6, false);

}

void AWordSearch::DestroyPuzzle()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Destroy function was called")));

	AActor* breakWordSearch = GetWorld()->SpawnActor<AActor>(breakWordActor, puzzleMesh->GetComponentLocation(), GetActorRotation());
	//breakWordSearch->SetActorScale3D(puzzleMesh->GetRelativeScale3D());
	Destroy();

}

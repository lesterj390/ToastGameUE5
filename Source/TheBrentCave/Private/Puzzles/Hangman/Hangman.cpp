// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Hangman/Hangman.h"

// Sets default values
AHangman::AHangman()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Mesh Components

	Hook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook Mesh"));
	Hook->SetupAttachment(RootComponent);

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head Mesh"));
	Head->SetupAttachment(RootComponent);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	Body->SetupAttachment(RootComponent);

	LeftArm = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Arm Mesh"));
	LeftArm->SetupAttachment(RootComponent);

	RightArm = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Arm Mesh"));
	RightArm->SetupAttachment(RootComponent);

	LeftLeg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Leg Mesh"));
	LeftLeg->SetupAttachment(RootComponent);

	RightLeg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Leg Mesh"));
	RightLeg->SetupAttachment(RootComponent);

	// Puzzle Components

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Puzzle Camera"));
	PuzzleCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(RootComponent);

	ChosenSentenceComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grid Widget"));
	ChosenSentenceComponent->SetupAttachment(RootComponent);

	inOverlap = false;
	inPuzzle = false;

	guesses = 0;
}

// Called when the game starts or when spawned
void AHangman::BeginPlay()
{
	Super::BeginPlay();

	PuzzleLight->SetVisibility(false);
	InteractComponent->SetVisibility(false);
	//ChosenSentenceComponent->SetVisibility(false);

	SetupInput();

	GenerateSentences();
}

// Called every frame
void AHangman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHangman::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AHangman::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Left Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
	}
}

void AHangman::Interact()
{
	if (inOverlap && !inPuzzle) {
		if (HitBoxPlayer)
		{
			inOverlap = true;
			inPuzzle = true;

			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
			InteractComponent->SetVisibility(false);
			PuzzleLight->SetVisibility(true);
			ChosenSentenceComponent->SetVisibility(true);

			HitBoxPlayer->SetActorHiddenInGame(true);
			HitBoxPlayer->SetActorEnableCollision(false);

			EnableInput(GetWorld()->GetFirstPlayerController());

			HangmanInputWidget->AddToViewport();
		}
	}
}

void AHangman::ExitPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = false;
		inOverlap = true;

		PuzzleLight->SetVisibility(false);
		HitBoxPlayer->SetActorHiddenInGame(false);
		HitBoxPlayer->SetActorEnableCollision(true);
		InteractComponent->SetVisibility(true);

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);
	}

	if (HangmanInputWidget) {
		HangmanInputWidget->RemoveFromParent();
	}
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

void AHangman::GuessCharacter(FString PGuess)
{
	FString sGuess = PGuess.ToUpper();
	char guess = sGuess[0];

	// Don't count any already guessed words as a guess
	if (!ChosenSentenceWidget->AttemptedLetters.Contains(guess)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Not Dup!")));

		// Check if your guess isn't correct
		if (!GuessedCorrect(guess)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Wrong Guess!")));

			guesses++;
			
			WrongGuess();
		}
	}

	ChosenSentenceWidget->AttemptedLetters.Add(guess);
	ChosenSentenceWidget->SetSentence(ChosenSentence);

	if (ChosenSentenceWidget->playerSentence == ChosenSentence) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Win!")));

		ExitPuzzle();
	}
}

void AHangman::SetupInput()
{
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AHangman::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &AHangman::OnOverlapEnd);

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &AHangman::Interact);
}

void AHangman::GenerateWidget()
{
	FSoftClassPath widgetClassRef(TEXT("/Game/_Main/Puzzles/Hangman/Blueprint/BP_HangmanWidget.BP_HangmanWidget_C"));

	if (UClass* widgetClass = widgetClassRef.TryLoadClass<UHangmanWidget>()) {
		ChosenSentenceComponent->SetWidgetClass(widgetClass);
		ChosenSentenceWidget = Cast<UHangmanWidget>(ChosenSentenceComponent->GetWidget());

		ChosenSentenceWidget->GenerateWidget(ChosenSentence);
	}

	FSoftClassPath inputClassRef(TEXT("/Game/_Main/Puzzles/Hangman/Blueprint/BP_HangmanInputWidget.BP_HangmanInputWidget_C"));

	if (UClass* inputClass = inputClassRef.TryLoadClass<UHangmanInputWidget>()) {
		HangmanInputWidget = CreateWidget<UHangmanInputWidget>(GetWorld(), inputClass);
		HangmanInputWidget->OnTextCleared.AddDynamic(this, &AHangman::ExitPuzzle);
	}
}

void AHangman::GenerateSentences()
{

	FString dir = FPaths::ProjectContentDir();
	FString myFile = dir + "/_Main/Puzzles/Hangman/Blueprint/sentences.txt";

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Directory: %s"), *dir));


	if (FPaths::FileExists(myFile)) {

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Found File!")));
		FFileHelper::LoadFileToStringArray(sentences, *myFile);

		if (sentences.Num() > 0) {

			ChosenSentence = sentences[FMath::RandRange(0, sentences.Num() - 1)];
			ChosenSentence = ChosenSentence.ToUpper();

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%s"), *ChosenSentence));

			GenerateWidget();

		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Problem With Sentence File!")));
		}
	}
}

void AHangman::WrongGuess()
{
	if (guesses == 1 && RightLeg) {
		RightLeg->SetSimulatePhysics(true);
	}
	else if (guesses == 2 && RightLeg && LeftLeg) {
		RightLeg->DestroyComponent();

		LeftLeg->SetSimulatePhysics(true);
	}
	else if (guesses == 3 && LeftLeg && RightArm) {
		LeftLeg->DestroyComponent();

		RightArm->SetSimulatePhysics(true);
	}
	else if (guesses == 4 && RightArm && LeftArm) {
		RightArm->DestroyComponent();

		LeftArm->SetSimulatePhysics(true);
	}
	else if (guesses == 5 && LeftArm && Body) {
		LeftArm->DestroyComponent();

		Body->SetSimulatePhysics(true);
	}
	else if (Body && Head) {
		Body->DestroyComponent();

		Head->SetSimulatePhysics(true);
	}
}

bool AHangman::GuessedCorrect(char PGuess)
{
	for (int i = 0; i < ChosenSentence.Len(); i++) {
		if (ChosenSentence[i] == PGuess) {
			return true;
		}
	}
	return false;
}

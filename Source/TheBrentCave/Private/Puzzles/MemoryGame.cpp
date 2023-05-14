// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/MemoryGame.h"
#include <iostream>
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMemoryGame::AMemoryGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	memoryBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Memory Base"));
	memoryBase->SetupAttachment(RootComponent);

	memoryTopLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Memory Top Left"));
	memoryTopLeft->SetupAttachment(RootComponent);

	memoryTopRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Memory Top Right"));
	memoryTopRight->SetupAttachment(RootComponent);

	memoryBottomLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Memory Bottom Left"));
	memoryBottomLeft->SetupAttachment(RootComponent);

	memoryBottomRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Memory Bottom Right"));
	memoryBottomRight->SetupAttachment(RootComponent);

	MemoryCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("KeyFrame Camera"));
	MemoryCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(RootComponent);

	inMemory = false;
	showingCombo = false;

	selected = 1;

	currentIndex = 0;

	currentPart = 0;

	displayAm = 1;

	isDestroyCalled = false;

	//Load Sounds.
	static ConstructorHelpers::FObjectFinder<USoundCue> topLeftSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/topLeftSound.topLeftSound'"));
	if(topLeftSoundObject.Succeeded())
	{
		topLeftSound = topLeftSoundObject.Object;

		topLeftAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("topLeftAudioComponent"));
		topLeftAudioComponent->SetupAttachment(RootComponent);

	}
	static ConstructorHelpers::FObjectFinder<USoundCue> topRightSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/topRightSound.topRightSound'"));
	if (topRightSoundObject.Succeeded())
	{
		topRightSound = topRightSoundObject.Object;

		topRightAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("topRightAudioComponent"));
		topRightAudioComponent->SetupAttachment(RootComponent);

	}
	static ConstructorHelpers::FObjectFinder<USoundCue> bottomLeftSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/bottomLeftSound.bottomLeftSound'"));
	if (bottomLeftSoundObject.Succeeded())
	{
		bottomLeftSound = bottomLeftSoundObject.Object;

		bottomLeftAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("bottomLeftAudioComponent"));
		bottomLeftAudioComponent->SetupAttachment(RootComponent);

	}
	static ConstructorHelpers::FObjectFinder<USoundCue> bottomRightSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/bottomRightSound.bottomRightSound'"));
	if (bottomRightSoundObject.Succeeded())
	{
		bottomRightSound = bottomRightSoundObject.Object;

		bottomRightAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("bottomRightAudioComponent"));
		bottomRightAudioComponent->SetupAttachment(RootComponent);

	}

	/*static ConstructorHelpers::FObjectFinder<USoundCue> selectedSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/memorySelectingSound.memorySelectingSound'"));
	if (selectedSoundObject.Succeeded())
	{
		selectedSound = selectedSoundObject.Object;

		selectedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("selectedAudioComponent"));
		selectedAudioComponent->SetupAttachment(RootComponent);

	}*/

	static ConstructorHelpers::FObjectFinder<USoundCue> pressedSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/memoryPressedSound.memoryPressedSound'"));
	if (pressedSoundObject.Succeeded())
	{
		pressedSound = pressedSoundObject.Object;

		pressedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("pressedAudioComponent"));
		pressedAudioComponent->SetupAttachment(RootComponent);

	}
	static ConstructorHelpers::FObjectFinder<USoundCue> wrongSoundObject(TEXT("SoundCue'/Game/_Main/Puzzles/Memory/Sounds/pressedWrongSound.pressedWrongSound'"));
	if (wrongSoundObject.Succeeded())
	{
		wrongSound = wrongSoundObject.Object;

		wrongAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("wrongAudioComponent"));
		wrongAudioComponent->SetupAttachment(RootComponent);

	}

	//Setup Box Collision.
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	PuzzleLight->SetVisibility(false);
	InteractComponent->SetVisibility(false);

	inOverlap = false;

}

void AMemoryGame::setupInput()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &AMemoryGame::InteractPuzzle);
	InputComponent->BindAction("Use", IE_Pressed, this, &AMemoryGame::SelectButton);
	DisableInput(GetWorld()->GetFirstPlayerController());
}

void AMemoryGame::EnterPuzzle()
{
	if (inOverlap == true) {
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
		PuzzleLight->SetVisibility(true);
		HitBoxPlayer->SetActorHiddenInGame(true);
		InteractComponent->SetVisibility(false);
		HitBoxPlayer->SetActorEnableCollision(false);
		EnableInput(GetWorld()->GetFirstPlayerController());
		inOverlap = true;

		inMemory = true;
		showingCombo = true;
		//selected = 1;
		wrongTimer();
	}
}

void AMemoryGame::ExitPuzzle()
{
	if (inOverlap == true || inOverlap == false) {

		if (inMemory && HitBoxPlayer) {

			PuzzleLight->SetVisibility(false);
			HitBoxPlayer->SetActorHiddenInGame(false);
			InteractComponent->SetVisibility(true);
			HitBoxPlayer->SetActorEnableCollision(true);

			inMemory = false;
			showingCombo = false;

			selected = 5;

			currentIndex = 0;

			currentPart = 0;

			displayAm = 1;

			topLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			topRightMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), 0);

			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);

		}
	}
}

void AMemoryGame::InteractPuzzle()
{
	if (inMemory) {
		ExitPuzzle();
	}
	else {
		EnterPuzzle();
	}
}


void AMemoryGame::SelectButton() {
	if (inMemory == true) {
		if (currentPart < pattern.Num() && selected == pattern[currentPart]) {
			//Correct Button Press.
			currentIndex = 0;
			if (currentPart + 1 >= displayAm) {
				displayAm++;
				currentPart = 0;
				displayNextIndex();
			}
			else {
				currentPart++;
				if (pressedAudioComponent && pressedSound) {
					pressedAudioComponent->Play(0.f);

				}

			}

		}
		else {
			//Incorrect Button Press.
			if (pressedAudioComponent && pressedSound) {
				pressedAudioComponent->Play(0.f);

			}

			selected = 5;

			GetWorldTimerManager().SetTimer(timerWrong, this, &AMemoryGame::wrongTimer, ResetTimer, false);

		}

	}
}

void AMemoryGame::displayNextIndex()
{

	if (currentIndex < displayAm && currentIndex < pattern.Num()) {

		selected = 5;

		GetWorldTimerManager().SetTimer(timerPause, this, &AMemoryGame::pauseTimer, TimerBetween, false);

	}
	else {

		showingCombo = false;
		selected = 5;
		GetWorldTimerManager().SetTimer(timerPause, this, &AMemoryGame::pauseTimer, TimerBetween, false);

	}

}

void AMemoryGame::pauseTimer()
{

	showingCombo = true;

	if (currentIndex < displayAm && currentIndex < pattern.Num()) {

		selected = pattern[currentIndex];

		if (selected == 1) {

			if (topLeftAudioComponent && topLeftSound) {

				topLeftAudioComponent->Play(0.f);

			}
			

		} else if (selected == 2) {
		
			if (topRightAudioComponent && topRightSound) {

				topRightAudioComponent->Play(0.f);

			}

		}
		else if (selected == 3) {

			if (bottomLeftAudioComponent && bottomLeftSound) {

				bottomLeftAudioComponent->Play(0.f);

			}

		}
		else if (selected == 4) {

			if (bottomRightAudioComponent && bottomRightSound) {

				bottomRightAudioComponent->Play(0.f);

			}

		}

		currentIndex++;

		GetWorldTimerManager().SetTimer(timerHandle, this, &AMemoryGame::displayNextIndex, NewColorTimer, false);

	}
	else {

		selected = 1;
		showingCombo = false;

	}
}

void AMemoryGame::wrongTimer()
{
	pattern.Empty();

	for (int i = 0; i < loopAm; i++) {
		pattern.Add(FMath::RandRange(1, 4));

		selected = 1;

		currentIndex = 0;

		currentPart = 0;

		displayAm = 1;

		showingCombo = true;

		displayNextIndex();
	}

}

void AMemoryGame::breakTimer()
{

	AActor* breakMemory = GetWorld()->SpawnActor<AActor>(breakMemoryActor, memoryBase->GetComponentLocation(), GetActorRotation());
	Destroy();

}

void AMemoryGame::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
			}
		}
	}
}


void AMemoryGame::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Exited Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//HitBoxPlayer->InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Called when the game starts or when spawned
void AMemoryGame::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMemoryGame::OnOverlapStart);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AMemoryGame::OnOverlapEnd);
	FSoftClassPath widgetClassRef(TEXT("/Game/_Main/Puzzles/KeyFrame/KeyWidget.KeyWidget_C"));

	if (topLeftAudioComponent && topLeftSound) {
		topLeftAudioComponent->SetSound(topLeftSound);
	}
	if (topRightAudioComponent && topRightSound) {
		topRightAudioComponent->SetSound(topRightSound);
	}
	if (bottomLeftAudioComponent && bottomLeftSound) {
		bottomLeftAudioComponent->SetSound(bottomLeftSound);
	}
	if (bottomRightAudioComponent && bottomRightSound) {
		bottomRightAudioComponent->SetSound(bottomRightSound);
	}
	/*if (selectedAudioComponent && selectedSound) {
		selectedAudioComponent->SetSound(selectedSound);
	}*/
	if (pressedAudioComponent && pressedSound) {
		pressedAudioComponent->SetSound(pressedSound);
	}
	if (wrongAudioComponent && wrongSound) {
		wrongAudioComponent->SetSound(wrongSound);
	}

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/InteractionWidget.InteractionWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {

		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = InteractComponent->GetWidget();

	}

	setupInput();

	tempMat = memoryTopLeft->GetMaterial(0);
	topLeftMat = UMaterialInstanceDynamic::Create(tempMat, this);
	memoryTopLeft->SetMaterial(0, topLeftMat);

	tempMat = memoryTopRight->GetMaterial(0);
	topRightMat = UMaterialInstanceDynamic::Create(tempMat, this);
	memoryTopRight->SetMaterial(0, topRightMat);

	tempMat = memoryBottomLeft->GetMaterial(0);
	bottomLeftMat = UMaterialInstanceDynamic::Create(tempMat, this);
	memoryBottomLeft->SetMaterial(0, bottomLeftMat);

	tempMat = memoryBottomRight->GetMaterial(0);
	bottomRightMat = UMaterialInstanceDynamic::Create(tempMat, this);
	memoryBottomRight->SetMaterial(0, bottomRightMat);

	for (int i = 0; i < loopAm; i++) {
		pattern.Add(FMath::RandRange(1, 4));
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%d"), pattern[i]));
	}

}

// Called every frame
void AMemoryGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (loopAm < displayAm && !isDestroyCalled) {

		isDestroyCalled = true;

		/////////////////////////////////////////////////////GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(,0.5f)

		PuzzleLight->SetVisibility(false);
		if (HitBoxPlayer)
		{
			//HitBoxPlayer->SetActorHiddenInGame(false);

			//GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);

			ExitPuzzle();
			GetWorldTimerManager().SetTimer(timerBreak, this, &AMemoryGame::breakTimer, 0.5, false);
		}
	}

	//Handles selection visibility.
	if (inMemory == true) {

		if (selected == 1) {

			//Stops glowing if not selected.
			topRightMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), 0);

			//Changes selection to visible display that the piece is selected.
			topLeftMat->SetScalarParameterValue(TEXT("Glow"), GlowStrength);


			if (showingCombo == false) {

				//Moves the selection to new piece depending on which arrow key is pressed.
				if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right)) {

					selected = 2;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
				else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down)) {

					selected = 3;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
			}
		}
		else if (selected == 2) {

			//Stops glowing if not selected.
			topLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), 0);

			//Changes selection to visible display that the piece is selected.
			topRightMat->SetScalarParameterValue(TEXT("Glow"), GlowStrength);

			if (showingCombo == false) {
				//Moves the selection to new piece depending on which arrow key is pressed.
				if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left)) {

					selected = 1;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
				else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Down)) {

					selected = 4;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
			}
		}
		else if (selected == 3) {

			//Stops glowing if not selected.
			topLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			topRightMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), 0);

			//Changes selection to visible display that the piece is selected.
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), GlowStrength);

			if (showingCombo == false) {
				//Moves the selection to new piece depending on which arrow key is pressed.
				if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Right)) {

					selected = 4;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
				else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up)) {

					selected = 1;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
			}

		}
		else if (selected == 4)
		{
			//Stops glowing if not selected.
			topLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			topRightMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);

			//Changes selection to visible display that the piece is selected.
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), GlowStrength);

			if (showingCombo == false) {

				//Moves the selection to new piece depending on which arrow key is pressed.
				if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Left)) {

					selected = 3;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
				else if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Up)) {

					selected = 2;
					/*if (selectedAudioComponent && selectedSound) {

						selectedAudioComponent->Play(0.f);

					}*/

				}
			}
		}

		else {

			topLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			topRightMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomLeftMat->SetScalarParameterValue(TEXT("Glow"), 0);
			bottomRightMat->SetScalarParameterValue(TEXT("Glow"), 0);

		}

	}

}


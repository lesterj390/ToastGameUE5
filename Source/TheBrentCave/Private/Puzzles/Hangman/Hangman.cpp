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

	ChosenSentenceComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grid Widget"));
	ChosenSentenceComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHangman::BeginPlay()
{
	Super::BeginPlay();

	GenerateSentences();
}

// Called every frame
void AHangman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

			//ChosenSentenceWidget->SetSentence(ChosenSentence);

		}
	}
}
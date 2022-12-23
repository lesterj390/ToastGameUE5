// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/Jigsaw/JigsawPiece.h"

// Sets default values
AJigsawPiece::AJigsawPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	Piece = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piece Mesh"));
	Piece->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AJigsawPiece::BeginPlay()
{
	Super::BeginPlay();
	
	//Fill = NewObject<TSubclassOf<UStaticMeshComponent>>(this, TSubclassOf<UStaticMeshComponent>::StaticClass(), TEXT("Fill Mesh"));

	//if (Fill) {

		//Fill->RegisterComponent();
		//Fill->AttachToComponent(Pivot, FAttachmentTransformRules::KeepRelativeTransform);
		//Fill->CreationMethod = EComponentCreationMethod::Instance;


	//}

}

// Called every frame
void AJigsawPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorKey.h"

// Sets default values
ADoorKey::ADoorKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorKeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorKey Mesh"));
	DoorKeyMesh->SetupAttachment(RootComponent);

	keysComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Keys Widget"));
	keysComponent->SetupAttachment(RootComponent);

	UseWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Use Widget"));
	UseWidgetComponent->SetupAttachment(RootComponent);

	EquipKeysComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Equip Keys Widget"));
	EquipKeysComponent->SetupAttachment(RootComponent);

	//Setup Box Collision.
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	keysComponent->SetVisibility(false);
	EquipKeysComponent->SetVisibility(false);
	UseWidgetComponent->SetVisibility(false);

	inOverlap = false;

}

// Called when the game starts or when spawned
void ADoorKey::BeginPlay()
{
	Super::BeginPlay();

	AActor* FoundGrid = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	grid = Cast<AGrid>(FoundGrid);

	FSoftClassPath keysClassRef(TEXT("/Game/_Main/DoorKey/Blueprint/DoorKeyWidget.DoorKeyWidget_C"));

	if (UClass* widgetClass = keysClassRef.TryLoadClass<UUserWidget>()) {

		keysComponent->SetWidgetClass(widgetClass);
		doorKeyWidget = keysComponent->GetWidget();

	}

	FSoftClassPath useClassRef(TEXT("/Game/_Main/UI/UseWidget.UseWidget_C"));

	if (UClass* widgetClass = useClassRef.TryLoadClass<UUserWidget>()) {

		UseWidgetComponent->SetWidgetClass(widgetClass);
		UseWidget = UseWidgetComponent->GetWidget();

	}

	FSoftClassPath equipClassRef(TEXT("/Game/_Main/DoorKey/Blueprint/NeedMoreKeysWidget.NeedMoreKeysWidget_C"));

	if (UClass* widgetClass = equipClassRef.TryLoadClass<UUserWidget>()) {

		EquipKeysComponent->SetWidgetClass(widgetClass);
		EquipKeysWidget = EquipKeysComponent->GetWidget();

	}

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ADoorKey::OnOverlapStart);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ADoorKey::OnOverlapEnd);

	keysComponent->SetVisibility(false);
	EquipKeysComponent->SetVisibility(false);
	UseWidgetComponent->SetVisibility(false);

	puzzleAm = grid->PuzzleActorArray.Num();

	updateWidget();
	setupInput();

}

// Called every frame
void ADoorKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorKey::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

				Player = Cast<ATBCCharacter>(FoundActors[0]);

				if (Player->KeyCount > 0 && Player->selectedItem == 3) {
					inOverlap = true;
					keysComponent->SetVisibility(true);
					UseWidgetComponent->SetVisibility(true);
				}
				else {
					EquipKeysComponent->SetVisibility(true);
				}

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");

			}
		}
	}
}

void ADoorKey::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	keysComponent->SetVisibility(false);
	EquipKeysComponent->SetVisibility(false);
	UseWidgetComponent->SetVisibility(false);

	inOverlap = false;
	DisableInput(GetWorld()->GetFirstPlayerController());

}

void ADoorKey::insertKey()
{

	if (inOverlap) {
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Overlap worked!")));
		if (Player && Player->KeyCount > 0)
		{
			Player->KeyCount--;
			puzzleAm--;

			UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), InsertKeySound);
			SoundComponent->Play(0.0);

			updateWidget();

			if (puzzleAm == 0)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Door unlocked")));

				AActor* breakDoorKey = GetWorld()->SpawnActor<AActor>(DoorKeyBreak, DoorKeyMesh->GetComponentLocation(), GetActorRotation());
				Player->RemoveExitWall();
				Destroy();
			}
		}
	}
}

void ADoorKey::updateWidget()
{

	UTextBlock* keysNeeded = (UTextBlock*)(doorKeyWidget->WidgetTree->FindWidget(FName(TEXT("keysNeeded"))));
	UTextBlock* neededText = (UTextBlock*)(doorKeyWidget->WidgetTree->FindWidget(FName(TEXT("neededText"))));

	if (keysNeeded) {
		FString Keys = FString::Printf(TEXT("%d"), puzzleAm);
		keysNeeded->SetText(FText::FromString(Keys));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("An error occured while modifying Widget")));
	}

	if (neededText) {

		if (keysNeeded && puzzleAm == 1) {
			FString Keys = FString::Printf(TEXT("key needed"));
			neededText->SetText(FText::FromString(Keys));
		}
		else {

			FString Keys = FString::Printf(TEXT("keys needed"));
			neededText->SetText(FText::FromString(Keys));

		}
	}

}

void ADoorKey::setupInput()
{

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Use", IE_Pressed, this, &ADoorKey::insertKey);
	DisableInput(GetWorld()->GetFirstPlayerController());

}


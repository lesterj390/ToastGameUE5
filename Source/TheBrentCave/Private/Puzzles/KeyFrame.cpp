// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/KeyFrame.h"

// Sets default values
AKeyFrame::AKeyFrame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyFrame Mesh"));
	Mesh->SetupAttachment(RootComponent);


	//WidgetPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Widget Pivot"));
	//WidgetPivot->SetupAttachment(RootComponent);


	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Key Widget"));
	WidgetComponent->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	KeyFrameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("KeyFrame Camera"));
	KeyFrameCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(RootComponent);

	InteractComponent->SetVisibility(false);

}


// Called when the game starts or when spawned
void AKeyFrame::BeginPlay()
{
	Super::BeginPlay();


	SetupInputComponent();
	PuzzleLight->SetVisibility(false);
	WidgetComponent->SetVisibility(false);

	InPuzzle = false;

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AKeyFrame::OnOverlapStart);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AKeyFrame::OnOverlapEnd);
	FSoftClassPath widgetClassRef(TEXT("/Game/_Main/Puzzles/KeyFrame/KeyWidget.KeyWidget_C"));

	if (UClass* widgetClass = widgetClassRef.TryLoadClass<UUserWidget>()) {
		WidgetComponent->SetWidgetClass(widgetClass);
		KeyWidget = WidgetComponent->GetWidget();

	}

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/InteractionWidget.InteractionWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {
		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = WidgetComponent->GetWidget();
	}

	//Removing Action Bindings from alphabet
	interactString = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->GetKeysForAction("Interact")[0].Key.GetFName().ToString();
	exitString = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->GetKeysForAction("Exit")[0].Key.GetFName().ToString();
	//resetString = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->GetKeysForAction("Reset")[0].Key.GetFName().ToString();

	for (int i = 0; i < 26; ++i) {
		if (alphabetStatic[i] != interactString[0] && alphabetStatic[i] != exitString[0]) {
			alphabet.Add(alphabetStatic[i]);
		}
	}

}


void AKeyFrame::SetupInputComponent()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	InputComponent->BindAction("Interact", IE_Pressed, this, &AKeyFrame::Interact);
	InputComponent->BindAction("Exit", IE_Pressed, this, &AKeyFrame::Exit);
	//InputComponent->BindAction("Reset", IE_Pressed, this, &AKeyFrame::TimerFunction);
	DisableInput(GetWorld()->GetFirstPlayerController());
}


void AKeyFrame::ClearKeyBindings(TArray<FInputKeyBinding> * KeyBindings)
{
	/*FInputChord ChordToRemove = FInputChord(KeyToRemove);

	for (int i = 0; i < KeyBindings->Num(); ++i) {
		if ((*KeyBindings)[i].Chord == ChordToRemove) {
			FString keyName = (*KeyBindings)[i].Chord.GetKeyText().ToString();
			KeyBindings->RemoveAt(i);
			break;
		}
	}*/
	KeyBindings->Empty();
}


void AKeyFrame::BindIncorrectKeys()
{
	FName loopKeyName;
	FName correctKeyName;
	FKey loopKey;

	FName interact = *interactString;
	FName exit = *exitString;
	for (int i = 65; i < 91; ++i) {
		loopKeyName = FName(*FString::Printf(TEXT("%c"), char(i)));
		loopKey = FKey(loopKeyName);
		correctKeyName = currentKey.GetFName();

		if (loopKeyName.Compare(correctKeyName) != 0 && loopKeyName.Compare(interact) != 0 && loopKeyName.Compare(exit) != 0) {
			InputComponent->BindKey(loopKey, IE_Pressed, this, &AKeyFrame::TimerFunction);
			UE_LOG(LogTemp, Warning, TEXT("Key Bound: %c"), char(i));
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Amount Keys Bound: %d"), InputComponent->KeyBindings.Num());
}


void AKeyFrame::TimerFunction()
{
	//Interact();
	RemainingKeys = NumberOfKeys;
	RemainingTime = TimePerKey;
	RemainingCountdown = CountdownSeconds;
	CountDown();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Failed Puzzle")));

}


void AKeyFrame::CountDown()
{
	if (RemainingCountdown == 0) {
		DisplayNewKey();
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AKeyFrame::TimerFunction, RemainingTime, false, RemainingTime);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Time To Press: %f"), RemainingTime));
	} else {
		UTextBlock* TextControl = (UTextBlock*)(KeyWidget->WidgetTree->FindWidget(FName(TEXT("Key"))));

		if (TextControl) {
			FString Letter = FString::Printf(TEXT("%d"), RemainingCountdown);
			TextControl->SetText(FText::FromString(Letter));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("An error occured while modifying Widget")));
		}

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AKeyFrame::CountDown, 1.0f, false, 1.0f);
	}
	RemainingCountdown--;
}


void AKeyFrame::Interact()
{
	if (!InPuzzle) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Entering Puzzle")));

		//Setting Key
		RemainingKeys = NumberOfKeys;
		RemainingTime = TimePerKey;
		RemainingCountdown = CountdownSeconds;
		//DisplayNewKey();

		WidgetComponent->SetVisibility(true);
		PuzzleLight->SetVisibility(true);
		if (HitBoxPlayer)
		{
			HitBoxPlayer->SetActorHiddenInGame(true);
			HitBoxPlayer->SetActorEnableCollision(false);
		}
		EnableInput(GetWorld()->GetFirstPlayerController());
	
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
		CountDown();
		//Timer Related Code
		/*GetWorldTimerManager().SetTimer(TimerHandle, this, &AKeyFrame::TimerFunction, RemainingTime, false, RemainingTime);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Time To Press: %f"), RemainingTime));*/
		InPuzzle = true;

		InteractComponent->SetVisibility(false);

	}
}


void AKeyFrame::Exit()
{
	WidgetComponent->SetVisibility(false);
	PuzzleLight->SetVisibility(false);
	if (HitBoxPlayer) {
		HitBoxPlayer->SetActorHiddenInGame(false);
		HitBoxPlayer->SetActorEnableCollision(true);
	}
	GetWorldTimerManager().ClearTimer(TimerHandle);
	ClearKeyBindings(&InputComponent->KeyBindings);
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);
	InPuzzle = false;

	InteractComponent->SetVisibility(true);

}


void AKeyFrame::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Entered Hitbox")));
	HitBoxPlayer = Cast<ATBCCharacter>(OtherActor);
	if (HitBoxPlayer) {
		ClearKeyBindings(&InputComponent->KeyBindings);

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
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
			}
		}
	}
}


void AKeyFrame::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Exited Hitbox")));
	if (HitBoxPlayer) {
		ClearKeyBindings(&InputComponent->KeyBindings);
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//HitBoxPlayer->InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
	}
}


void AKeyFrame::DisplayNewKey()
{
	UTextBlock* TextControl = (UTextBlock*)(KeyWidget->WidgetTree->FindWidget(FName(TEXT("Key"))));

	if (TextControl) {
		FString Letter = FString::Printf(TEXT("%c"), alphabet[FMath::RandRange(0, alphabet.Num()-1)]);
		currentKey = FKey(FName(*Letter));
		currentKeyBinding = InputComponent->BindKey(currentKey, IE_Pressed, this, &AKeyFrame::KeyPress);
		BindIncorrectKeys();
		TextControl->SetText(FText::FromString(Letter));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("An error occured while modifying Widget")));
	}
}


void AKeyFrame::KeyPress()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	RemainingKeys--;
	RemainingTime *= 0.5;
	ClearKeyBindings(&InputComponent->KeyBindings);
	if (RemainingKeys > 0) {
		
		DisplayNewKey();

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AKeyFrame::TimerFunction, RemainingTime, false, RemainingTime);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Time To Press: %f"), RemainingTime));
	}
	else {

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Victory")));
		Exit();

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AKeyFrame::FinishPuzzle, 0.5, false);

	}
}


void AKeyFrame::FinishPuzzle()
{

	AActor* KeyFrameBreak = GetWorld()->SpawnActor<AActor>(breakKeyFrameActor, Mesh->GetComponentLocation(), GetActorRotation());
	//KeyFrameBreak->SetActorScale3D(GetActorScale3D());
	Destroy();

}


// Called every frame
void AKeyFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




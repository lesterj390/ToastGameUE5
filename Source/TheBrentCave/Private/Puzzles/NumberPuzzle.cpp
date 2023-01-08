// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/NumberPuzzle.h"

// Sets default values
ANumberPuzzle::ANumberPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	numberBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Number Base"));
	numberBase->SetupAttachment(Pivot);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("KeyFrame Camera"));
	PuzzleCamera->SetupAttachment(RootComponent);

	PuzzleLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Puzzle Light"));
	PuzzleLight->SetupAttachment(RootComponent);

	ShapeComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Shape Widget"));
	ShapeComponent->SetupAttachment(Pivot);

	VertComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Vert Widget"));
	VertComponent->SetupAttachment(Pivot);

	HoriComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hori Widget"));
	HoriComponent->SetupAttachment(Pivot);

	NumberInputComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Input Widget"));
	NumberInputComponent->SetupAttachment(Pivot);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(Pivot);

	HintComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hint Widget"));
	HintComponent->SetupAttachment(Pivot);

	//Setup Box Collision.
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	inOverlap = false;
	inPuzzle = false;

	PuzzleLight->SetVisibility(false);
	ShapeComponent->SetVisibility(false);
	VertComponent->SetVisibility(false);
	HoriComponent->SetVisibility(false);
	HintComponent->SetVisibility(false);
	NumberInputComponent->SetVisibility(false);
	InteractComponent->SetVisibility(false);

}

// Called when the game starts or when spawned
void ANumberPuzzle::BeginPlay()
{
	Super::BeginPlay();

	FSoftClassPath shapeClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/ShapeWidget.ShapeWidget_C"));

	if (UClass* widgetClass = shapeClassRef.TryLoadClass<UUserWidget>()) {

		ShapeComponent->SetWidgetClass(widgetClass);
		ShapeWidget = ShapeComponent->GetWidget();

	}
	
	FSoftClassPath vertClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/VertWidget.VertWidget_C"));

	if (UClass* widgetClass = vertClassRef.TryLoadClass<UUserWidget>()) {

		VertComponent->SetWidgetClass(widgetClass);
		VertWidget = VertComponent->GetWidget();

	}

	FSoftClassPath horiClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/HoriWidget.HoriWidget_C"));

	if (UClass* widgetClass = horiClassRef.TryLoadClass<UUserWidget>()) {

		HoriComponent->SetWidgetClass(widgetClass);
		HoriWidget = HoriComponent->GetWidget();

	}

	FSoftClassPath inputClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/NumberInput.NumberInput_C"));

	if (UClass* widgetClass = inputClassRef.TryLoadClass<UUserWidget>()) {

		NumberInputComponent->SetWidgetClass(widgetClass);
		InputWidget = NumberInputComponent->GetWidget();

	}

	FSoftClassPath promptClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/InputUI.InputUI_C"));

	if (UClass* widgetClass = promptClassRef.TryLoadClass<UUserWidget>()) {

		PromptWidget = CreateWidget<UUserWidget>(GetWorld(), widgetClass);

	}

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/InteractionWidget.InteractionWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {

		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = InteractComponent->GetWidget();

	}

	FSoftClassPath hintClassRef(TEXT("/Game/_Main/Puzzles/Number/Blueprint/HintWidget.HintWidget_C"));

	if (UClass* widgetClass = hintClassRef.TryLoadClass<UUserWidget>()) {

		HintComponent->SetWidgetClass(widgetClass);
		HintWidget = HintComponent->GetWidget();

	}

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANumberPuzzle::OnOverlapStart);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ANumberPuzzle::OnOverlapEnd);

	/*if (ShapeWidget) {
		ShapeWidget->AddToViewport();
	}*/

	/*if (VertWidget) {
		VertWidget->AddToViewport();
	}*/

	/*if (HoriWidget) {
		HoriWidget->AddToViewport();
	}*/

	exitString = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerInput->GetKeysForAction("Exit")[0].Key.GetFName().ToString();

	randomShapeSetup();
	GetHint();

	setupInput();

}

// Called every frame
void ANumberPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANumberPuzzle::randomShapeSetup()
{

	for (int i = 0; i < 4; i++) {

		//circle = 0
		//square = 1
		//triangle = 2
		//diamond = 3

		shapeVal[i] = (FMath::RandRange(1, 9));

	}

	for (int i = 0; i < 16; i++) {

		UImage* imageType = (UImage*)(ShapeWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("shape%d"), i + 1))));
		FSoftObjectPath softObjectPath;
		softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/circle.circle'"));
		UTexture2D* circle = Cast<UTexture2D>(softObjectPath.TryLoad());
		softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/square.square'"));
		UTexture2D* square = Cast<UTexture2D>(softObjectPath.TryLoad());
		softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/triangle.triangle'"));
		UTexture2D* triangle = Cast<UTexture2D>(softObjectPath.TryLoad());
		softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/diamond.diamond'"));
		UTexture2D* diamond = Cast<UTexture2D>(softObjectPath.TryLoad());

		if (imageType) {
			int currentShape = (FMath::RandRange(1, 4));

			if (currentShape == 1) {
				imageType->SetBrushFromTexture(circle);
				shapes[i] = shapeVal[0];
			}
			else if (currentShape == 2) {
				imageType->SetBrushFromTexture(square);
				shapes[i] = shapeVal[1];
			}
			else if (currentShape == 3) {
				imageType->SetBrushFromTexture(triangle);
				shapes[i] = shapeVal[2];
			}
			else if (currentShape == 4) {
				imageType->SetBrushFromTexture(diamond);
				shapes[i] = shapeVal[3];
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("An error occured while modifying Widget")));
		}

	}

	for (int i = 0; i < 4; i++) {

		UTextBlock* TextControl = (UTextBlock*)(VertWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("num%d"), i + 1))));
		FString Number = FString::Printf(TEXT("%d"), shapes[i * 4] + shapes[i * 4 + 1] + shapes[i * 4 + 2] + shapes[i * 4 + 3]);
		TextControl->SetText(FText::FromString(Number));

	}

	for (int i = 0; i < 4; i++) {

		UTextBlock* TextControl = (UTextBlock*)(HoriWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("num%d"), i + 1))));
		FString Number = FString::Printf(TEXT("%d"), shapes[i] + shapes[i + 4] + shapes[i + 8] + shapes[i + 12]);
		TextControl->SetText(FText::FromString(Number));

	}

	answer = shapes[0] + shapes[5] + shapes[10] + shapes[15];

}

void ANumberPuzzle::GetHint()
{
	// Select Random Shape
	int randomShape = FMath::RandRange(0, 3);

	// Set Image
	UImage* hintImage = (UImage*)(HintWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("HintImage")))));
	// Get Image References
	FSoftObjectPath softObjectPath;
	softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/circle.circle'"));
	UTexture2D* circle = Cast<UTexture2D>(softObjectPath.TryLoad());
	softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/square.square'"));
	UTexture2D* square = Cast<UTexture2D>(softObjectPath.TryLoad());
	softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/triangle.triangle'"));
	UTexture2D* triangle = Cast<UTexture2D>(softObjectPath.TryLoad());
	softObjectPath.SetPath(TEXT("Texture2D'/Game/_Main/Puzzles/Number/Textures/diamond.diamond'"));
	UTexture2D* diamond = Cast<UTexture2D>(softObjectPath.TryLoad());
	// Apply Image
	if (randomShape == 0) {
		hintImage->SetBrushFromTexture(circle);
	}
	else if (randomShape == 1) {
		hintImage->SetBrushFromTexture(square);
	}
	else if (randomShape == 2) {
		hintImage->SetBrushFromTexture(triangle);
	}
	else if (randomShape == 3) {
		hintImage->SetBrushFromTexture(diamond);
	}

	// Set Text
	UTextBlock* TextControl = (UTextBlock*)(HintWidget->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("HintText")))));
	FString Number = FString::Printf(TEXT("%d"), shapeVal[randomShape]);
	TextControl->SetText(FText::FromString(Number));
}

void ANumberPuzzle::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATBCCharacter * OtherTBCCharacter = Cast<ATBCCharacter>(OtherActor);
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

void ANumberPuzzle::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	inOverlap = false;
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//HitBoxPlayer->InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
	}

}


void ANumberPuzzle::setupInput()
{

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &ANumberPuzzle::enterPuzzle);
	EnableInput(GetWorld()->GetFirstPlayerController());
	InputComponent->BindAction("Exit", IE_Pressed, this, &ANumberPuzzle::exitPuzzle);
	EnableInput(GetWorld()->GetFirstPlayerController());

}

void ANumberPuzzle::enterPuzzle()
{

	if (inOverlap == true && inPuzzle == false) {
		if (HitBoxPlayer)
		{
			inPuzzle = true;

			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
			PuzzleLight->SetVisibility(true);

			HitBoxPlayer->SetActorHiddenInGame(true);
			ShapeComponent->SetVisibility(true);
			VertComponent->SetVisibility(true);
			HoriComponent->SetVisibility(true);
			HintComponent->SetVisibility(true);
			NumberInputComponent->SetVisibility(true);
			InteractComponent->SetVisibility(false);

			if (PromptWidget) {

				PromptWidget->AddToViewport();

			}
		}
		
	}

}

void ANumberPuzzle::exitPuzzle()
{

	if (inOverlap == true && inPuzzle == true) {

		if (PromptWidget) {

			PromptWidget->RemoveFromViewport();

		}

		if (HitBoxPlayer)
		{
			inPuzzle = false;

			PuzzleLight->SetVisibility(false);
			HitBoxPlayer->SetActorHiddenInGame(false);
			ShapeComponent->SetVisibility(false);
			VertComponent->SetVisibility(false);
			HoriComponent->SetVisibility(false);
			HintComponent->SetVisibility(false);
			NumberInputComponent->SetVisibility(false);
			InteractComponent->SetVisibility(true);


			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);
		}
		//AActor::SetFocus(HitboxPlayer);

	}

}

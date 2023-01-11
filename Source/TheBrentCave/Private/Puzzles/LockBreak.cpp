// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/LockBreak.h"

// Sets default values
ALockBreak::ALockBreak()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	ChestTopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Top Mesh"));
	ChestTopMesh->SetupAttachment(Pivot);

	ChestBottomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Bottom Mesh"));
	ChestBottomMesh->SetupAttachment(Pivot);

	LockBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Base Mesh"));
	LockBaseMesh->SetupAttachment(Pivot);

	LockDialMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Dial Mesh"));
	LockDialMesh->SetupAttachment(Pivot);

	LockHookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Hook Mesh"));
	LockHookMesh->SetupAttachment(Pivot);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(Pivot);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PuzzleCamera->SetupAttachment(Pivot);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(Pivot);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(Pivot);

	LockLevel = 1;

	ComboRecentlyPlayed = false;

	clockwise = true;

	inOverlap = false;
	inPuzzle = false;
}

// Called when the game starts or when spawned
void ALockBreak::BeginPlay()
{
	Super::BeginPlay();
	
	Spotlight->SetVisibility(false);
	InteractComponent->SetVisibility(false);
	ResetDialTimeline.SetLooping(false);

	SetupPuzzle();
}

void ALockBreak::SetupTimelines()
{
	// Puzzle Timeline
	// Setup Curve
	if (DialCurve) {
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("UpdateTimelineProgress"));
		DialTimeline.AddInterpFloat(DialCurve, TimelineProgress);
	}

	// Reset Puzzle Timeline
	// Setup Curve
	if (ResetDialCurve) {
		FOnTimelineFloat ResetTimelineProgress;
		ResetTimelineProgress.BindUFunction(this, FName("UpdateResetTimelineProgress"));
		ResetDialTimeline.AddInterpFloat(ResetDialCurve, ResetTimelineProgress);
	}

	// Setup On Complete Event
	FOnTimelineEvent ResetTimelineComplete;
	ResetTimelineComplete.BindUFunction(this, FName("ResetLock"));
	ResetDialTimeline.SetTimelineFinishedFunc(ResetTimelineComplete);
}

void ALockBreak::UpdateTimelineProgress(float Value)
{
	float angle = FMath::Lerp(0, 359, Value);
	DialLocation = angle;
	LockDialMesh->SetRelativeRotation((FRotator(angle, 90.0f, 0.0f)));

	if (LockLevel <= LockLength) {
		if (DialLocation >= LockCode[LockLevel - 1] && DialLocation <= LockCode[LockLevel - 1] + PickRange) {
			if (!ComboRecentlyPlayed) {
				ComboRecentlyPlayed = true;

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "On Combo");

				UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), OnComboSound);
				SoundComponent->Play(0.0);
			}
		}
		else {
			ComboRecentlyPlayed = false;
		}
	}
}

void ALockBreak::UpdateResetTimelineProgress(float Value)
{
	float angle = FMath::Lerp(0, 1080, Value);
	LockDialMesh->SetRelativeRotation((FRotator(angle, 90.0f, 0.0f)));
}

void ALockBreak::SetupPuzzle()
{
	SetupTimelines();
	SetupPlayerInput();
	GenerateLockCode();
}

void ALockBreak::SetupPlayerInput()
{
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &ALockBreak::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &ALockBreak::OnOverlapEnd);

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &ALockBreak::Interact);
	InputComponent->BindAction("Exit", IE_Pressed, this, &ALockBreak::ExitPuzzle);
	DisableInput(GetWorld()->GetFirstPlayerController());
}

void ALockBreak::Interact()
{
	if (inPuzzle) {
		LockPickCheck();
	}
	else {
		EnterPuzzle();
	}
}

void ALockBreak::EnterPuzzle()
{
	inPuzzle = true;

	DialTimeline.Stop();
	ResetDialTimeline.Stop();

	ResetDialTimeline.PlayFromStart();

	if (HitBoxPlayer)
	{
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.5f);
		InteractComponent->SetVisibility(false);
		Spotlight->SetVisibility(true);

		HitBoxPlayer->SetActorHiddenInGame(true);
		HitBoxPlayer->SetActorEnableCollision(false);
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void ALockBreak::ExitPuzzle()
{
	if (HitBoxPlayer)
	{
		inPuzzle = false;

		Spotlight->SetVisibility(false);
		HitBoxPlayer->SetActorHiddenInGame(false);
		HitBoxPlayer->SetActorEnableCollision(true);
		InteractComponent->SetVisibility(true);

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.5f);

		ResetLock();

		// Stop Ticking Sound
		GetWorldTimerManager().ClearTimer(TickTimer);
	}
}

// Makes a tick noise 
void ALockBreak::TickNoise()
{
	// Creates the 2d sound component for the tick noise
	UAudioComponent* SoundComponent = UGameplayStatics::CreateSound2D(GetWorld(), TickSound);
	// Plays tick noise
	SoundComponent->Play(0.0);
}

void ALockBreak::ResetLock()
{
	ResetDialTimeline.Stop();
	DialTimeline.Stop();

	// Stop fast ticking
	GetWorldTimerManager().ClearTimer(TickTimer);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Function ResetLock was called.");

	LockLevel = 1;
	clockwise = true;

	ComboRecentlyPlayed = false;

	if (inPuzzle) {
		GetWorldTimerManager().SetTimer(ResetCooldown, this, &ALockBreak::StartPuzzle, 0.5, false);
	}
	else {
		LockDialMesh->SetRelativeRotation((FRotator(0.0f, 90.0f, 0.0f)));
	}
}

void ALockBreak::StartPuzzle()
{
	DialTimeline.PlayFromStart();
	DialTimeline.SetLooping(true);
	GetWorldTimerManager().SetTimer(TickTimer, this, &ALockBreak::TickNoise, TickNoiseDelay, true);
}

void ALockBreak::GenerateLockCode()
{
	LockCode.Empty();

	int MaxLockPick = 359 - PickRange;

	for (int i = 0; i < LockLength; i++) {
		LockCode.Add(FMath::RandRange(0, MaxLockPick));
	}
}

void ALockBreak::LockPickCheck()
{
	if (LockLevel <= LockLength) {
		if (DialLocation >= LockCode[LockLevel - 1] && DialLocation <= LockCode[LockLevel - 1] + PickRange) {
			if (clockwise) {
				DialTimeline.Reverse();
			}
			else {
				DialTimeline.Play();
			}
			clockwise = !clockwise;
			LockLevel++;
		}
		else {
			DialTimeline.Stop();
			ResetDialTimeline.Stop();

			ResetDialTimeline.PlayFromStart();
			// Stop normal ticking and start faster ticking
			GetWorldTimerManager().ClearTimer(TickTimer);
			GetWorldTimerManager().SetTimer(TickTimer, this, &ALockBreak::TickNoise, TickNoiseDelay/2, true);
		}

		if (LockLevel > LockLength) {
			// Win Puzzle
			GetWorldTimerManager().SetTimer(BreakPuzzleTimer, this, &ALockBreak::WinPuzzle, 0.6, false);
			ExitPuzzle();
		}
	}
}

void ALockBreak::WinPuzzle()
{
	// Spawn Destroy Puzzle Actor
	AActor* breakLockBreak = GetWorld()->SpawnActor<AActor>(BreakLockActor, ChestBottomMesh->GetComponentLocation(), GetActorRotation());

	// Destroy this
	Destroy();
}

// Called every frame
void ALockBreak::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DialTimeline.TickTimeline(DeltaTime);
	ResetDialTimeline.TickTimeline(DeltaTime);

}

void ALockBreak::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ALockBreak::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	inOverlap = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Left Hitbox")));
	if (HitBoxPlayer) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
	}
}
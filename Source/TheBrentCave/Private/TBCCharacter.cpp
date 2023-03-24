// Copyright Epic Games, Inc. All Rights Reserved.

#include "TBCCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Maze/DataTypes.h"
#include "Grid.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ATBCCharacter

ATBCCharacter::ATBCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), TEXT("head")); // Attach the camera to the mesh head
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Finding InteractWidgetSubclass
	static ConstructorHelpers::FClassFinder<UUserWidget> InteractWidgetFinder(TEXT("/Game/_Main/UI/InteractionWidget"));
	if (!ensure (InteractWidgetFinder.Class != nullptr)) return;

	InteractWidgetSubclass = InteractWidgetFinder.Class;
	// Creating InteractWidget
	InteractWidget = CreateWidget<UUserWidget>(GetWorld(), InteractWidgetSubclass);


	// Finding HintWidgetSubclass
	static ConstructorHelpers::FClassFinder<UUserWidget> HintWidgetFinder(TEXT("/Game/_Main/UI/HintWidget"));
	if (!ensure(HintWidgetFinder.Class != nullptr)) return;

	HintWidgetSubclass = HintWidgetFinder.Class;


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	PlayerStats = CreateDefaultSubobject<UPlayerStatsComponent>(TEXT("PlayerStatsComponent"));

	bIsSprinting = false;
	bStartedSprint = false;

	bIsRelaxing = false;
	bStartedRelax = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetMesh(), TEXT("head"));

	// Setup flashlight
	FlashlightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	FlashlightComponent->SetupAttachment(SpringArm);

	// Setup flashlight cone
	//FlashlightCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightCone"));
	//FlashlightCone->SetupAttachment(Flashlight);

	// Setup audio component pivot
	SoundPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SoundPivot"));
	SoundPivot->SetupAttachment(GetMesh(), TEXT("head"));

	KeyCount = 0;

	// Setup spawn point for glowstick
	glowstickThrowPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glowstick Spawn Point"));
	glowstickThrowPoint->SetupAttachment(RootComponent);

	BreadcrumbMount = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Breadcrumb Mount"));
	BreadcrumbMount->SetupAttachment(GetMesh(), TEXT("head"));
	//BreadcrumbMount->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	static ConstructorHelpers::FObjectFinder<USoundCue> whisperCueObject(TEXT("SoundCue'/Game/ThirdPersonCPP/Audio/Whisper/WhisperCue.WhisperCue'"));

	if (whisperCueObject.Succeeded())
	{
		WhisperSound = whisperCueObject.Object;
	}

	/*
	static ConstructorHelpers::FObjectFinder<USoundCue> backgroundCueObject(TEXT("SoundCue'/Game/ThirdPersonCPP/Audio/BackgroundNoise/backgroundNoise_Cue.backgroundNoise_Cue'"));

	if (backgroundCueObject.Succeeded())
	{
		BackgroundSound = backgroundCueObject.Object;
	}
	*/

	static ConstructorHelpers::FObjectFinder<USoundCue> flashlightCueObject(TEXT("SoundCue'/Game/ThirdPersonCPP/Audio/Flashlight/FlashlightToggle_Cue.FlashlightToggle_Cue'"));

	if (flashlightCueObject.Succeeded())
	{
		FlashlightToggleSound = flashlightCueObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> radarCueObject(TEXT("SoundCue'/Game/_Main/Radar/Audio/RadarSignal_Cue.RadarSignal_Cue'"));

	if (radarCueObject.Succeeded())
	{
		RadarSound = radarCueObject.Object;
	}

	hasRadar = false;

	isHiding = false;

	selectedItem = 0;

	pickedUpItem = false;

}

//////////////////////////////////////////////////////////////////////////
// Input

void ATBCCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATBCCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATBCCharacter::EndSprint);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATBCCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATBCCharacter::MoveRight);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ATBCCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATBCCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATBCCharacter::LookUpAtRate);

	// handle touch devices
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &ATBCCharacter::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &ATBCCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATBCCharacter::OnResetVR);

	//Toggle for flashlight
	PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &ATBCCharacter::flashlightToggle);

	//Binding for relaxing (sanity regeneration)
	PlayerInputComponent->BindAction("Relax", IE_Pressed, this, &ATBCCharacter::StartRelax);
	PlayerInputComponent->BindAction("Relax", IE_Released, this, &ATBCCharacter::EndRelax);

	//Binding for hints
	PlayerInputComponent->BindAction("Hint", IE_Pressed, this, &ATBCCharacter::ToggleHint);

	//Button for throwing using item
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ATBCCharacter::UseItem);

	//Scroll Inputs for changing item in inventory
	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &ATBCCharacter::ScrolledUp);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &ATBCCharacter::ScrolledDown);

	//Insert battery into different items
	PlayerInputComponent->BindKey(FKey(TEXT("One")), IE_Pressed, this, &ATBCCharacter::BatteryInFlashlight);
	PlayerInputComponent->BindKey(FKey(TEXT("Two")), IE_Pressed, this, &ATBCCharacter::BatteryInRadar);
}

void ATBCCharacter::UpdateRadarDistance()
{

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToastClass, FoundActors);
	if (FoundActors.Num() > 0) {
		ToastRef = FoundActors[0];

		if (selectedItem == Radar && GetDistanceTo(ToastRef) < 10000 && RadarOn && RadarBattery > 0) {

			float DistanceToToast = GetDistanceTo(ToastRef) / 10000;
			
			RadarDistance = FMath::Lerp(0.1, 2, DistanceToToast);
			RadarPitch = FMath::Lerp(1.5, 0.2, DistanceToToast);

			UAudioComponent* RadarComponent = UGameplayStatics::CreateSound2D(GetWorld(), RadarSound);
			RadarComponent->SetPitchMultiplier(RadarPitch);

			RadarComponent->Play(0.0);

			GetWorldTimerManager().ClearTimer(RaderSignalTimer);
			GetWorldTimerManager().SetTimer(RaderSignalTimer, this, &ATBCCharacter::UpdateRadarDistance, RadarDistance, false);
		}
		else if (selectedItem == Radar && GetDistanceTo(ToastRef) > 10000 && RadarOn && RadarBattery > 0) {

			GetWorldTimerManager().ClearTimer(RaderSignalTimer);
			GetWorldTimerManager().SetTimer(RaderSignalTimer, this, &ATBCCharacter::UpdateRadarDistance, 0.5, false);

		}
		else {
			RadarOn = false;
		}
	}

}

void ATBCCharacter::OnResetVR()
{
	// If TP_ThirdPerson is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TP_ThirdPerson.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


void ATBCCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATBCCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}


void ATBCCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val);

	if (FlashlightComponent) {
		FlashlightComponent->SetWorldRotation(GetControlRotation());
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), Val));
	}

	if (BreadcrumbMount) {
		BreadcrumbMount->SetWorldRotation(GetControlRotation());
	}
	
}


void ATBCCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), GetCharacterMovement()->MaxWalkSpeed));

	}
	
	if (Value == 0.0f && rightVal == 0.0f) {
		//Ended sprint
		if (bStartedSprint) {
			PlayerStats->RegenerateStamina();
			bStartedSprint = false;

			if (SprintingAudioComponent) {
				SprintingAudioComponent->FadeOut(0.5, 0.0, EAudioFaderCurve::Linear);
			}
		}

		//Started relax
		if (bIsRelaxing && !bStartedRelax)
		{
			PlayerStats->RegenerateSanity();
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0, 1, 1.5, FColor::Black, true, true);
			bStartedRelax = true;
		}
	}
	else {
		//Started sprint
		if (bIsSprinting && !bStartedSprint) {
			PlayerStats->ConsumeStamina();
			bStartedSprint = true;

			if (SprintingAudioComponent) {
				SprintingAudioComponent->Play(0.0);
				SprintingAudioComponent->FadeIn(0.5, 1.0, 0.0, EAudioFaderCurve::Linear);
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Error Finding Reference to SprintingAudioComponent")));

				SprintingAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), SprintingAudio);
				SprintingAudioComponent->Play(0.0);
				SprintingAudioComponent->FadeIn(0.5, 1.0, 0.0, EAudioFaderCurve::Linear);
			}
		}

		//Ended relax
		if (bStartedRelax)
		{
			PlayerStats->ConsumeSanity();
			bStartedRelax = false;
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1, 0, 0.25, FColor::Black, true, true);
		}
	}
}


void ATBCCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}

	rightVal = Value;
}


void ATBCCharacter::UseItem()
{
	if (selectedItem == Flashlight) {
		flashlightToggle();
	}
	else if (selectedItem == Glowstick) {

		throwGlowstick();

		UpdateInvBar();

	}
	else if (selectedItem == Food) {

		if (FoodAmount > 0) {
			EatCrumbReturn = GetWorld()->SpawnActor<AActor>(EatCrumbActor, GetActorLocation(), FRotator(GetActorRotation()));
			EatCrumbReturn->AttachToComponent(BreadcrumbMount, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			FoodAmount--;

			UpdateInvBar();

			if (FoodAmount == 0) {
				ScrolledUp();
			}

		}

	}
	else if (selectedItem == Battery) {

		if (BatteryAmount > 0 && !hasRadar) {

			BatteryAmount--;
			BatteryPower = MaxBatteryCharge;

			if (BatteryAmount == 0) {
				ScrolledUp();
			}

			GetWorldTimerManager().ClearTimer(BatteryDrain);
			GetWorldTimerManager().SetTimer(BatteryDrain, this, &ATBCCharacter::ReduceBattery, BatteryReductionSpeed, true);

			UpdateInvBar();

			if (!flashlightOn) {
				flashlightToggle();
			}

		}

	}
	else if (selectedItem == Radar) {

		if (RadarBattery > 0 && !RadarOn) {
			RadarOn = true;

			ReduceRadarBattery();

			UpdateRadarDistance();
		}
		else if (RadarOn) {
			RadarOn = false;
		}
	}
}


void ATBCCharacter::ScrolledUp()
{

	selectedItem++;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%d"), selectedItem));

	if (UPlayerStatsWidget* StatsWidget = Cast<UPlayerStatsWidget>(Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)->PlayerStatsWidget)) {

		if (selectedItem != Battery) {
			StatsWidget->BatteryBox->SetVisibility(ESlateVisibility::Hidden);
		}

		if (selectedItem == Flashlight) {
			FString Path = FString("/Game/_Main/UI/SelectedItem/FlashlightRing.FlashlightRing");
			//UTexture2D* FlashlightTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

			StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->FlashlightTexture);

			InventroyProgressBar = BatteryPower / 100;
		}
		else if (selectedItem == Glowstick) {
			if (GlowstickAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/GlowstickRing.GlowstickRing");
				//UTexture2D* GlowstickTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->GlowstickTexture);

				//InventroyProgressBar = GlowstickAmount / MaxGlowstickAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem++;
				ScrolledUp();
			}
		}
		else if (selectedItem == Food) {
			if (FoodAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/FoodRing.FoodRing");
				//UTexture2D* BreadTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->BreadTexture);

				//InventroyProgressBar = FoodAmount / MaxFoodAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem++;
				ScrolledUp();
			}
		}
		else if (selectedItem == Key) {
			if (KeyCount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/KeyRing.KeyRing");
				//UTexture2D* KeyTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->KeyTexture);

				//InventroyProgressBar = KeyCount / PuzzleCount;
				UpdateInvBar();
			}
			else {
				//selectedItem++;
				ScrolledUp();
			}
		}
		else if (selectedItem == Battery) {
			if (BatteryAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/BatteryRing.BatteryRing");
				//UTexture2D* BatteryTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->BatteryTexture);

				if (hasRadar) {
					StatsWidget->BatteryBox->SetVisibility(ESlateVisibility::Visible);
				}

				//InventroyProgressBar = BatteryAmount / MaxBatteryAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem++;
				ScrolledUp();
			}
		}
		else if (selectedItem == Radar) {
			if (hasRadar) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/RadarRing.RadarRing");
				//UTexture2D* RadarTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(StatsWidget->RadarTexture);
			}
			else {
				//selectedItem++;
				ScrolledUp();
			}
		}
		else if (selectedItem > Radar) {

			selectedItem = -1;
			ScrolledUp();

		}

		UpdateInvBar();

	}

	if (HintWidget && HintWidget->IsInViewport()) {
		ToggleHint();
	}

}

void ATBCCharacter::ScrolledDown()
{

	selectedItem--;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%d"), selectedItem));

	if (UPlayerStatsWidget* StatsWidget = Cast<UPlayerStatsWidget>(Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)->PlayerStatsWidget)) {

		if (selectedItem != 4) {
			StatsWidget->BatteryBox->SetVisibility(ESlateVisibility::Hidden);
		}

		if (selectedItem == Flashlight) {
			FString Path = FString("/Game/_Main/UI/SelectedItem/FlashlightRing.FlashlightRing");
			UTexture2D* FlashlightTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

			StatsWidget->SelectedItem->SetBrushFromTexture(FlashlightTexture);

			InventroyProgressBar = BatteryPower / 100;
		}
		else if (selectedItem == Glowstick) {
			if (GlowstickAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/GlowstickRing.GlowstickRing");
				UTexture2D* GlowstickTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(GlowstickTexture);

				//InventroyProgressBar = GlowstickAmount / MaxGlowstickAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem--;
				ScrolledDown();
			}
		}
		else if (selectedItem == Food) {
			if (FoodAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/FoodRing.FoodRing");
				UTexture2D* BreadTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(BreadTexture);

				//InventroyProgressBar = FoodAmount / MaxFoodAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem--;
				ScrolledDown();
			}
		}
		else if (selectedItem == Key) {
			if (KeyCount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/KeyRing.KeyRing");
				UTexture2D* KeyTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(KeyTexture);

				//InventroyProgressBar = KeyCount / PuzzleCount;
				UpdateInvBar();
			}
			else {
				//selectedItem--;
				ScrolledDown();
			}
		}
		else if (selectedItem == Battery) {
			if (BatteryAmount > 0) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/BatteryRing.BatteryRing");
				UTexture2D* BatteryTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(BatteryTexture);

				if (hasRadar) {
					StatsWidget->BatteryBox->SetVisibility(ESlateVisibility::Visible);
				}

				//InventroyProgressBar = BatteryAmount / MaxBatteryAmount;
				UpdateInvBar();
			}
			else {
				//selectedItem--;
				ScrolledDown();
			}
		}
		else if (selectedItem == Radar) {
			if (hasRadar) {
				FString Path = FString("/Game/_Main/UI/SelectedItem/RadarRing.RadarRing");
				UTexture2D* RadarTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

				StatsWidget->SelectedItem->SetBrushFromTexture(RadarTexture);
			}
			else {
				//selectedItem++;
				ScrolledDown();
			}
		}
		else if (selectedItem < 0) {

			selectedItem = 6;
			ScrolledDown();

		}
	
		UpdateInvBar();

	}

	if (HintWidget && HintWidget->IsInViewport()) {
		ToggleHint();
	}

}

void ATBCCharacter::BeginPlay()
{

	Super::BeginPlay();

	MyGameInstance = Cast<UGISetup>(UGameplayStatics::GetGameInstance(GetWorld()));
	//MySavedSettings = Cast<USavedSettings>(UGameplayStatics::CreateSaveGameObject(USavedSettings::StaticClass()));
	//MySavedSettings = Cast<USavedSettings>(UGameplayStatics::LoadGameFromSlot(FString::Printf("SaveSettings"), 0));
	//USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(FString::Printf("SaveSettings"), 0);

	BatteryPower = MaxBatteryCharge;
	RadarBattery = MaxRadarBattery;

	AActor* FoundGrid = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	grid = Cast<AGrid>(FoundGrid);

	if (grid) {
		PuzzleCount = grid->PuzzleActorArray.Num();
	}

	for (int i = 0; i < AmbienceArray.Num(); i++) {
		BackgroundSound = AmbienceArray[i];

		UAudioComponent* BackgroundComponent = UGameplayStatics::CreateSound2D(GetWorld(), BackgroundSound);
		BackgroundComponent->Play(0.0);
	}

	// Define sprinting audio as component
	SprintingAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), SprintingAudio);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	

	DefaultIntensity = FlashlightComponent->Intensity;


	//Spawning Character in the middle of the Maze
	AActor* myActor = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (myActor != NULL) {
		auto myGrid = Cast<AGrid>(myActor);

		MazeCenter = myGrid->GetActorLocation();
		MazeCenter.Y += (myGrid->Rows) / 2 * 1000 * myGrid->GetActorScale3D().Y;
		MazeCenter.X += (myGrid->Columns) / 2 * 1000 * myGrid->GetActorScale3D().X;
		MazeCenter.Z = GetActorLocation().Z;

		SetActorLocation(MazeCenter);

		// Adding Player Tag Number
		myGrid->spawnedPlayers++;
		Tags.Add(*FString::Printf(TEXT("Player%d"), myGrid->spawnedPlayers));
	}


	//UE_LOG(LogTemp, Warning, TEXT("Found widget : %s"), *InteractWidgetSubclass->GetName());

	//Setting up interaction widget
	if (InteractWidget)
	{
		InteractWidget->AddToViewport();
		InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
	}

	//Setting up player stats

	// Sprinting
	PlayerStats->SetRegenerationRate(StaminaRegenerationRate);	
	PlayerStats->SetConsumptionRate(StaminaConsumptionRate);
	PlayerStats->SetCooldown(StaminaCooldown);
	PlayerStats->SetUpdateRate(UpdateRate);
	LastStaminaState = PlayerStats->GetStaminaState();
	// Sanity
	PlayerStats->SetSanityDuration(SanityDuration);
	PlayerStats->SetSanityRegenerationDuration(SanityRegenerationDuration);

	//Setting up sanity consumption
	PlayerStats->ConsumeSanity();

	//Setting up flashlight
	defaultLightFlickerMin = LightFlickerMin;
	defaultLightFlickerMax = LightFlickerMax;
	flashlightOn = false;
	FlashlightComponent->SetVisibility(false);
	flashlightToggle();

	GetWorldTimerManager().SetTimer(sanityHandle, this, &ATBCCharacter::SanityCheck, .25, true);
	GetWorldTimerManager().SetTimer(BatteryDrain, this, &ATBCCharacter::ReduceBattery, BatteryReductionSpeed, true);
}


void ATBCCharacter::Tick(float DeltaTime) 
{	
	if (pickedUpItem == true) {
		pickedUpItem = false;

		UpdateInvBar();
	}

	// Checking if I'm in a puzzle
	UClass* viewClass = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetViewTarget()->GetClass();
	if (viewClass->IsChildOf(ATBCCharacter::StaticClass()) || isHiding) {
		bInPuzzle = false;

		APlayerCameraManager* CurrentCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		CurrentCamera->SetFOV(MyGameInstance->PlayerFOV);
	}
	else {
		bInPuzzle = true;

		APlayerCameraManager* CurrentCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		CurrentCamera->SetFOV(MyGameInstance->PuzzleFOV);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("InPuzzle: %d"), bInPuzzle));

	// This code detects whether I've left or entered a puzzle / locker
	// It's used to remove the hint widget from the screen
	lastCameraClass = currentCameraClass;
	currentCameraClass = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetViewTarget()->GetClass();

	if (!currentCameraClass->IsChildOf(lastCameraClass)) {
		// Turns on or off footstep audio depending on if the player is leaving or entering a hiding place or puzzle
		if (FootstepRef) {
			ToggleFootstepAudio();
		}

		// If the hint widget is currently displayed
		if (HintWidget && HintWidget->IsInViewport()) {
			ToggleHint();
		}
	}
}

/**
* If the function is called non recursively, it starts a recursive loop of calling itself.
* If it is called recursively, it will simulate a flicker effect flickerCount/2 times.
* Author: Lester Chavez 
*/
void ATBCCharacter::displayFlicker(int flickerCount)
{
	float timeUntilNextFlicker;

	// If all the flickers have already happened or this function is being called for the first time
	if (flickerCount == 0) {
		// The time until next flicker is a random value between LightFlickerMin and LightFlickerMax
		// This random value will decrease as the battery of the flashlight decreases to a minumum value of LightFlickerMin / 2
		timeUntilNextFlicker = FMath::Lerp(LightFlickerMin / 2, FMath::RandRange(LightFlickerMin, LightFlickerMax), BatteryPower / 100);

		// Display 2 or 3 flickers
		flickerCount = FMath::RandRange(2, 3) * 2;

		// Creates a timer delegate to call this function again with a parameter
		FTimerDelegate beginFlicker = FTimerDelegate::CreateUObject(this, &ATBCCharacter::displayFlicker, flickerCount);
		GetWorld()->GetTimerManager().SetTimer(flickerHandle, beginFlicker, timeUntilNextFlicker, false);
	}
	// If this function is being recursively called to actually flicker
	else {
		// If the flicker count is even, dim the flashlight
		if (flickerCount % 2 == 0) {
			FlashlightComponent->SetIntensity(FMath::RandRange(0.0f, DefaultIntensity / 2));
		}
		// If the flicker count is odd, set the flashlight back to full brightness
		else {
			FlashlightComponent->SetIntensity(DefaultIntensity);
		}

		// Creates a timer delegate to call this function again with a parameter
		FTimerDelegate continueFlicker = FTimerDelegate::CreateUObject(this, &ATBCCharacter::displayFlicker, flickerCount - 1);
		GetWorld()->GetTimerManager().SetTimer(flickerHandle, continueFlicker, FMath::RandRange(0.05f, 0.2f), false);
	}
}


void ATBCCharacter::flashlightToggle()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Flashlight!")));

	UAudioComponent* FlashlightToggleComponent = UGameplayStatics::CreateSound2D(GetWorld(), FlashlightToggleSound);
	FlashlightToggleComponent->Play(0.0);

	if (flashlightOn == true) {

		FlashlightComponent->ToggleVisibility(false);
		flashlightOn = false;
		GetWorld()->GetTimerManager().ClearTimer(flickerHandle);
	}
	else if (!flashlightOn && BatteryPower > 0) {

		FlashlightComponent->ToggleVisibility(true);
		flashlightOn = true;

		ReduceBattery();

		// Start flickering at random times
		displayFlicker(0);

	}
}


void ATBCCharacter::CheckForCooldown()
{
	if (PlayerStats->CanSprint() == false)
	{
		EndSprint();
		GetWorldTimerManager().ClearTimer(CheckForCooldownTimer);
	}
}


FVector ATBCCharacter::CalculateWhisperLocation(float angle)
{
	float z = UKismetMathLibrary::DegSin(angle) * 185;
	float y = UKismetMathLibrary::DegCos(angle) * -185;
	return FVector(0, y, z);
}


void ATBCCharacter::InsanityWhisper(int multiplier)
{
	if (WhisperSound)
	{
		FVector WhisperLocation = CalculateWhisperLocation(FMath::RandRange(-115.0f, 115.0f));
		UAudioComponent* SoundComponent = UGameplayStatics::SpawnSoundAttached(WhisperSound, SoundPivot, NAME_None, WhisperLocation, EAttachLocation::KeepRelativeOffset, false, 1.0f, 1.0f, 0.0f, nullptr, nullptr, true);
		SoundComponent->SetVolumeMultiplier(FMath::RandRange(0.3f * multiplier, 1.0f * multiplier));
		SoundComponent->SetPitchMultiplier(FMath::RandRange(0.8f, 1.2f));
		if (SoundComponent)
		{
			SoundComponent->Play(0.0f);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Could'nt create whisperaudiocomponent")));
		}
	}
}


void ATBCCharacter::ToggleHint() 
{
	UClass* viewClass = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetViewTarget()->GetClass();
	FString className = viewClass->GetPathName();
	className = FPaths::GetBaseFilename(className);
	
	TArray<FString> stringWords;
	FString* hintStringReference;
	FString hintString;

	// Looking up the Inventory Hint Strings or Hint Strings map to find respective hint
	if (viewClass->IsChildOf(ATBCCharacter::StaticClass())) { // Inventory hints
		hintStringReference = InventoryHintStrings.Find((TEnumAsByte<InventoryItem>)selectedItem);
	}
	else { // Normal hints
		hintStringReference = HintStrings.Find(className);
	}

	// Finding hint if found
	if (hintStringReference == nullptr) // No hint found
	{
		hintString = FString("No hints found for this. Sorry!");
	}
	else // Hint found
	{
		hintString = *hintStringReference;

		// Replacing Action Names in hint with their respective key
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		TArray<FInputActionKeyMapping> ActionMappings = PlayerController->PlayerInput->ActionMappings;
		FString bindingName = "";
		FString keyName = "";
		FString lastBindingName = "";
		for (FInputActionKeyMapping actionBinding : ActionMappings) {
			lastBindingName = bindingName;
			bindingName = actionBinding.ActionName.ToString();
			keyName = actionBinding.Key.GetDisplayName().ToString();

			if (bindingName == lastBindingName) {
				// Skip duplicate keys
				continue;
			}

			hintString = hintString.Replace(*bindingName, *keyName, ESearchCase::CaseSensitive);
		}
		// Remove escape character
		hintString = hintString.Replace(TEXT("\\"), TEXT(""));
	}


	// Toggling between displaying and removing the hint widget
	if (HintWidget && HintWidget->IsInViewport()) {
		HintWidget->RemoveFromParent();
	}
	else
	{
		HintWidget = CreateWidget<UUserWidget>(GetWorld(), HintWidgetSubclass);
		HintWidget->AddToViewport();
		UTextBlock* HintTextBlock = (UTextBlock*)(HintWidget->WidgetTree->FindWidget(FName(TEXT("HintTextBlock"))));
		if (HintTextBlock != nullptr) {
			HintTextBlock->SetText(FText::FromString(hintString));
		}
	}
}

void ATBCCharacter::ToggleFootstepAudio()
{
	UClass* viewClass = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetViewTarget()->GetClass();
	FString className = viewClass->GetPathName();
	className = FPaths::GetBaseFilename(className);

	if (viewClass->IsChildOf(ATBCCharacter::StaticClass())) { // Player is in character camera
		FootstepRef->VolumeMultiplier = 0.5f;
	}
	else { // Player is outside of body
		FootstepRef->VolumeMultiplier = 0.0f;
	}
}


void ATBCCharacter::StartSprint()
{
	if (PlayerStats->CanSprint()) {
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetWorldTimerManager().SetTimer(CheckForCooldownTimer, this, &ATBCCharacter::CheckForCooldown, 1.0f, true);

	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}


void ATBCCharacter::EndSprint()
{
	if (bIsSprinting) {
		PlayerStats->RegenerateStamina();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bStartedSprint = false;

		if (SprintingAudioComponent) {
			SprintingAudioComponent->FadeOut(0.5, 0.0, EAudioFaderCurve::Linear);
		}
	}
	bIsSprinting = false;
}


void ATBCCharacter::StartRelax()
{
	if (bInPuzzle == false) {
		bIsRelaxing = true;
	}
}


void ATBCCharacter::EndRelax()
{
	bIsRelaxing = false;
	if (bStartedRelax)
	{
		PlayerStats->ConsumeSanity();
		bStartedRelax = false;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1, 0, 1, FColor::Black, true, true);
	}
}


void ATBCCharacter::SanityCheck()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), PlayerStats->GetSanity()));
	if (PlayerStats->GetSanity() <= 10)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity <= 10")));
		if ((int)FMath::RandRange(1, 4) == 2)
		{
			InsanityWhisper(1.75);
		}
		if ((int)FMath::RandRange(1, 20) == 10)
		{
			SummonBreadcrumb();
		}
		// disable stamina
		// toast random spawn chance
		InsanityCameraShake(FMath::RandRange(1.5f, 2.2f));
	}
	else if (PlayerStats->GetSanity() <= 25)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity <= 25")));
		// camera shake
		// halucinations
		if ((int)FMath::RandRange(1, 10) == 5)
		{
			InsanityWhisper(1.5);
		}
		if ((int)FMath::RandRange(1, 20) == 10)
		{
			SummonBreadcrumb();
		}
		InsanityCameraShake(FMath::RandRange(1.3f, 1.8f));
	}
	else if (PlayerStats->GetSanity() <= 50)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity <= 50")));
		if ((int)FMath::RandRange(1, 20) == 10)
		{
			InsanityWhisper(1);
		}
		if ((int)FMath::RandRange(1, 28) == 14)
		{
			SummonBreadcrumb();
		}
		InsanityCameraShake(1);
	}
	else if (PlayerStats->GetSanity() <= 75)
	{
		if ((int)FMath::RandRange(1, 40) == 20)
		{
			SummonBreadcrumb();
		}
		InsanityCameraShake(-1);
	}
	else
	{
		TArray<AActor*> FoundCrumbs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BreadcrumbActor, FoundCrumbs);

		for (AActor* Crumb : FoundCrumbs)
		{
			Crumb->Destroy();
		}
		InsanityCameraShake(-1);
	}
}


void ATBCCharacter::StaminaCheck()
{
	// If stamina state was changed
	if (PlayerStats->GetStaminaState() != LastStaminaState)
	{
		if (PlayerStats->GetStaminaState() == PlayerStats->IDLE)
		{

		}
		else if (PlayerStats->GetStaminaState() == PlayerStats->REGENERATING)
		{

		}
		else if (PlayerStats->GetStaminaState() == PlayerStats->CONSUMING)
		{

		}
		else if (PlayerStats->GetStaminaState() == PlayerStats->COOLDOWN)
		{

		}
	}
}


void ATBCCharacter::SummonBreadcrumb()
{
	FNavLocation SummonNavLocation;
	FVector SummonLocation;
	bool bFoundLocation = false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	bFoundLocation = NavSystem->GetRandomPointInNavigableRadius(MazeCenter, MazeCenter.Y/2, SummonNavLocation);


	if (bFoundLocation)
	{
		SummonLocation = SummonNavLocation;
		SummonLocation.Z = GetActorLocation().Z;
		if (BreadcrumbActor)
		{
			GetWorld()->SpawnActor<AActor>(BreadcrumbActor, SummonLocation, FRotator(0, 0, 0));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Loaded breadcrumb")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Error summoning breadcrumb")));
	}
}


void ATBCCharacter::throwGlowstick()
{

	if (GlowstickAmount != 0) {

		GlowstickAmount--;

		throwReturn = GetWorld()->SpawnActor<AActor>(GlowstickActor, glowstickThrowPoint->GetComponentLocation(), FRotator(glowstickThrowPoint->GetComponentRotation()));
		//AGlowstick* Glowstick = Cast<AGlowstick>(throwReturn);

		//glowstickDirection = GetActorLocation() - Glowstick->GetActorLocation();
		// = glowstickDirection * 50;

		//Glowstick->CapsuleCollision->SetPhysicsLinearVelocity(glowstickDirection);

		if (GlowstickAmount == 0) {
			ScrolledUp();
		}

	}
}


void ATBCCharacter::InsanityCameraShake(float scale)
{
	if (CameraShakeInstance == nullptr && !bInPuzzle) {
		CameraShakeInstance = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(CameraShake, scale);
	}
	else {
		if (scale == -1) {
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopCameraShake(CameraShakeInstance);
			CameraShakeInstance = nullptr;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("No more shake")));
		}
		// Stop shaking while in puzzle
		else if (bInPuzzle) {
			InsanityCameraShake(-1);
		}
		else {
			// Update camera shake scale
			FCameraShakeUpdateResult result;
			CameraShakeInstance->ApplyScale(scale, result);
		}
	}
}


void ATBCCharacter::ReduceBattery()
{
	if (BatteryPower > 0 && flashlightOn) {

		if (!IsHidden()) {
			BatteryPower -= BatteryReductionRate;
		}


		if (BatteryPower < 0) {
			BatteryPower = 0;
		}

		if (BatteryPower == 0) {
			if (flashlightOn) {
				FlashlightComponent->ToggleVisibility(false);
				flashlightOn = false;
			}
		}

		if (selectedItem == Flashlight) {
			InventroyProgressBar = BatteryPower / 100;
		}

		GetWorldTimerManager().SetTimer(BatteryDrain, this, &ATBCCharacter::ReduceBattery, BatteryReductionSpeed, true);
	}
}

void ATBCCharacter::ReduceRadarBattery()
{

	if (selectedItem == Radar && RadarBattery > 0 && RadarOn) {
		RadarBattery -= RadarUpdateRate;

		UpdateInvBar();

		GetWorldTimerManager().SetTimer(RadarBatteryDrain, this, &ATBCCharacter::ReduceRadarBattery, RadarUpdateRate, true);
	}

}

void ATBCCharacter::BatteryInFlashlight()
{
	if (selectedItem == Battery) {
		BatteryAmount--;
		BatteryPower = MaxBatteryCharge;

		if (BatteryAmount == 0) {
			ScrolledUp();
		}

		GetWorldTimerManager().ClearTimer(BatteryDrain);
		GetWorldTimerManager().SetTimer(BatteryDrain, this, &ATBCCharacter::ReduceBattery, BatteryReductionSpeed, true);

		UpdateInvBar();

		if (!flashlightOn) {
			flashlightToggle();
		}
	}
}

void ATBCCharacter::BatteryInRadar()
{
	if (selectedItem == Battery) {
		BatteryAmount--;
		RadarBattery = MaxRadarBattery;

		if (BatteryAmount == 0) {
			ScrolledUp();
		}

		UpdateRadarDistance();

		ReduceRadarBattery();

		UpdateInvBar();
	}
}

void ATBCCharacter::UpdateInvBar()
{

	if (selectedItem == Glowstick) {
		InventroyProgressBar = (float)GlowstickAmount / MaxGlowstickAmount;
	}
	else if (selectedItem == Food) {
		InventroyProgressBar = (float)FoodAmount / MaxFoodAmount;
	}
	else if (selectedItem == Key) {
		InventroyProgressBar = (float)KeyCount / PuzzleCount;
	}
	else if (selectedItem == Battery) {
		InventroyProgressBar = (float)BatteryAmount / MaxBatteryAmount;
	}
	else if (selectedItem == Radar) {
		InventroyProgressBar = (float)RadarBattery / MaxRadarBattery;
	}

}

void ATBCCharacter::RemoveExitWall()
{
	AActor* myActor = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	AGrid* myGrid;
	if (myActor != NULL) {
		myGrid = Cast<AGrid>(myActor);

		TArray<AActor*> FoundWalls;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), *FString::Printf(TEXT("(%d, %d)|(%d, %d)"), -1, myGrid->Columns / 2, 0, myGrid->Columns / 2), FoundWalls);

		if (FoundWalls.Num() >= 1)
		{
			//Summon Exit Path.
			AActor* ExitPath = GetWorld()->SpawnActor<AActor>(ExitActor, FoundWalls[0]->GetActorLocation(), FoundWalls[0]->GetActorRotation());
			AActor* ExitDoor = GetWorld()->SpawnActor<AActor>(ExitDoorActor, FoundWalls[0]->GetActorLocation(), FoundWalls[0]->GetActorRotation());
			ExitDoor->SetActorScale3D(GetActorScale3D());

			FoundWalls[0]->Destroy();
		}
	}
}

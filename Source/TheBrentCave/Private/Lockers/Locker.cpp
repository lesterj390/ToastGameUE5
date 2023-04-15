// Fill out your copyright notice in the Description page of Project Settings.


#include "Lockers/Locker.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
// Sets default values
ALocker::ALocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Open = false;
	//ReadyState = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LockerBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Locker Base"));
	LockerBase->SetupAttachment(RootComponent);

	Pivot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);

	LockerDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Locker Door"));
	LockerDoor->SetupAttachment(Pivot);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Hide Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

	LockerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Locker Camera"));
	LockerCamera->SetupAttachment(RootComponent);

	OuterLockerLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Outer Locker Light"));
	OuterLockerLight->SetupAttachment(LockerCamera);

	InnerLockerLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Inner Locker Light"));
	InnerLockerLight->SetupAttachment(LockerCamera);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(RootComponent);

	IsHiding = false;

	InteractComponent->SetVisibility(false);

	Yaw = 90;

}


// Called when the game starts or when spawned
void ALocker::BeginPlay()
{
	Super::BeginPlay();

	OuterLockerLight->SetVisibility(false);
	InnerLockerLight->SetVisibility(false);

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/HideWidget.HideWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {

		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = InteractComponent->GetWidget();
	}

	SetupInputComponent();
	SetupDoorCurve();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ALocker::OnOverlapStart);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ALocker::OnOverlapEnd);


	/*if (DoorCurve) {*/
	//	FOnTimelineFloat TimelineCallback;
	//	FOnTimelineEventStatic TimelineFinishedCallback;

	//	TimelineCallback.BindUFunction(this, FName("ControlDoor"));
	//	TimelineFinishedCallback.BindUFunction(this, FName("SetState"));

	//	MyTimeline = NewObject<UTimelineComponent>(this, FName("DoorTimeline"));
	//	MyTimeline->SetTimelineLength(0.4f);
	//	MyTimeline->AddInterpFloat(DoorCurve, TimelineCallback);
	//	MyTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
	//	MyTimeline->RegisterComponent();

	//}
	
}

// Called every frame
void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DoorTimeline.TickTimeline(DeltaTime);
	//MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
}
//
//void ALocker::ControlDoor() 
//{
//	TimelineValue = MyTimeline->GetPlaybackPosition();
//	CurveFloatValue = DoorCurve->GetFloatValue(TimelineValue);
//
//	FQuat NewRotation = FQuat(FRotator(0.0f, CurveFloatValue, 0.0f));
//
//	Pivot->SetRelativeRotation(NewRotation);
//}
//
//void ALocker::SetState()2
//{
//	ReadyState = true;
//}

void ALocker::OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("hitboxplayer: %s"), *(PlayerTag.ToString())));
			}
		}
	}
		
}

void ALocker::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "End Overlap");
	if (HitBoxPlayer && OtherActor->IsA(ATBCCharacter::StaticClass())) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//HitBoxPlayer->InteractWidget->GetWidgetFromName("interactprompt")->SetVisibility(ESlateVisibility::Hidden);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("Exitted Hitbox")));
	}
}



void ALocker::SetupInputComponent()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	InputComponent->BindAction("Interact", IE_Pressed, this, &ALocker::InteractLocker);
	DisableInput(GetWorld()->GetFirstPlayerController());
}

void ALocker::AddCameraYaw(float Val)
{
	if (LockerCamera) {
		FRotator CameraRot = LockerCamera->GetRelativeRotation();
		if (CameraRot.Yaw < 105.0f && Val > 0) {
			Yaw = CameraRot.Yaw + Val;
		}
		else if (CameraRot.Yaw > 75.0f && Val < 0) {
			Yaw = CameraRot.Yaw + Val;
		}
		LockerCamera->SetRelativeRotation(FRotator(CameraRot.Pitch, Yaw, CameraRot.Roll));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), Val));
	}
}

void ALocker::AddCameraPitch(float Val)
{
	if (LockerCamera) {
		FRotator CameraRot = LockerCamera->GetRelativeRotation();
		if (CameraRot.Pitch < 30.0f && Val < 0) {
			Pitch = CameraRot.Pitch - Val;
		}
		else if (CameraRot.Pitch > -40.0f && Val > 0) {
			Pitch = CameraRot.Pitch - Val;
		}
		LockerCamera->SetRelativeRotation(FRotator(Pitch, CameraRot.Yaw, CameraRot.Roll));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%f"), Val));
	}
}


void ALocker::EnterLocker()
{
	if (!IsHiding) {
		IsHiding = true;

		if (HitBoxPlayer) {
			HitBoxPlayer->bIsHiding = true;
			InteractComponent->SetVisibility(false);

			HitBoxPlayer->SetActorEnableCollision(false);
			DoorTimeline.PlayFromStart();

			OuterLockerLight->SetVisibility(true);
			InnerLockerLight->SetVisibility(true);
			HitBoxPlayer->SetActorHiddenInGame(true);
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.4f);
			InputComponent->BindAxis("Turn", this, &ALocker::AddCameraYaw);
			InputComponent->BindAxis("LookUp", this, &ALocker::AddCameraPitch);
			EnableInput(GetWorld()->GetFirstPlayerController());
		}
	}
}

void ALocker::LeaveLocker()
{
	if (IsHiding) {
		IsHiding = false;

		if (HitBoxPlayer) {
			HitBoxPlayer->bIsHiding = false;
			InteractComponent->SetVisibility(true);

			DoorTimeline.PlayFromStart();

			OuterLockerLight->SetVisibility(false);
			InnerLockerLight->SetVisibility(false);
			HitBoxPlayer->SetActorHiddenInGame(false);
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.6f);
			InputComponent->AxisBindings.Empty();
			HitBoxPlayer->SetActorEnableCollision(true);
		}
		//DisableInput(GetWorld()->GetFirstPlayerController());
	}
}


void ALocker::InteractLocker()
{
	if (IsHiding) {
		LeaveLocker();
	}
	else {
		EnterLocker();
	}
}


void ALocker::SetupDoorCurve()
{
	if (DoorCurve) {
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("UpdateTimelineProgress"));
		DoorTimeline.AddInterpFloat(DoorCurve, TimelineProgress);
	}
}

void ALocker::UpdateTimelineProgress(float Value)
{
	float angle = FMath::Lerp(ClosedDoorAngle, OpenDoorAngle, Value);
	Pivot->SetRelativeRotation((FRotator(0.0f, angle, 0.0f)));
}

//void ALocker::ToggleDoor()
//{
//	if (ReadyState) {
//		Open = !Open;
//
//		ReadyState = false;
//
//		if (Open) {
//			MyTimeline->PlayFromStart();
//			//UE_LOG(LogTemp, Warning, TEXT("Hey There, i played in reverse"));
//
//		}
//
//		else {
//			MyTimeline->Reverse();
//		}
//		
//
//	}
//}


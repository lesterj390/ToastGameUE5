// Fill out your copyright notice in the Description page of Project Settings.


#include "Glowstick.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AGlowstick::AGlowstick()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	CapsuleCollision->SetupAttachment(RootComponent);
	CapsuleCollision->SetCollisionProfileName(TEXT("BlockAll"));

	Glowstick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glowstick Mesh"));
	Glowstick->SetupAttachment(CapsuleCollision);

	stickLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glowstick Light"));
	stickLight->SetupAttachment(CapsuleCollision);

	setupMat = false;

}

// Called when the game starts or when spawned
void AGlowstick::BeginPlay()
{
	Super::BeginPlay();

	//CapsuleCollision->MoveIgnoreActors.Add(PlayerCharacter);

	tempMat = Glowstick->GetMaterial(0);
	glowstickMat = UMaterialInstanceDynamic::Create(tempMat, this);
	Glowstick->SetMaterial(0, glowstickMat);

	glowColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));

	glowstickMat->SetVectorParameterValue(TEXT("GlowstickColor"), glowColor);
	glowstickMat->SetScalarParameterValue(TEXT("Glow"), 50);
	stickLight->SetLightColor(glowColor);

	SetupFadeCurve();

}

// Called every frame
void AGlowstick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fadeTimeline.TickTimeline(DeltaTime);

}

void AGlowstick::UpdateTimelineProgress(float Value)
{

	float brightnessLight = FMath::Lerp(5000, 0, Value);
	stickLight->SetIntensity(brightnessLight);

	float brightnessMat = FMath::Lerp(50, 0, Value);
	glowstickMat->SetScalarParameterValue(TEXT("Glow"), brightnessMat);

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%d"), brightnessLight));

}

void AGlowstick::SetupFadeCurve()
{

	if (fadeCurve) {

		FOnTimelineFloat timelineProgress;
		timelineProgress.BindUFunction(this, FName("UpdateTimelineProgress"));
		fadeTimeline.AddInterpFloat(fadeCurve, timelineProgress);

		FOnTimelineEvent timelineEvent;
		timelineEvent.BindUFunction(this, FName("FinishTimeline"));
		fadeTimeline.SetTimelineFinishedFunc(timelineEvent);

		fadeTimeline.PlayFromStart();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Started Timeline");

	}

}

void AGlowstick::FinishTimeline()
{

	Destroy();

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/KeyReward.h"

// Sets default values
AKeyReward::AKeyReward()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	keyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Key Mesh"));
	keyMesh->SetupAttachment(RootComponent);

	keyHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	keyHitbox->SetupAttachment(keyMesh);
	keyHitbox->SetCollisionProfileName(TEXT("Trigger"));

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(keyMesh);

	SmokeBurst = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Burst Particle"));
	SmokeBurst->SetupAttachment(keyMesh);

	InteractComponent->SetVisibility(false);
}

void AKeyReward::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
			}
		}
	}
}

void AKeyReward::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (KeyWidget) {
		DisableInput(GetWorld()->GetFirstPlayerController());
		InteractComponent->SetVisibility(false);
		//KeyWidget->GetWidgetFromName("PickupPrompt")->SetVisibility(ESlateVisibility::Hidden);
	}

}

void AKeyReward::updateWidget()
{



}

// Called when the game starts or when spawned
void AKeyReward::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();

	if (WidgetSubclass) {
		KeyWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetSubclass);
	}

	keyHitbox->OnComponentBeginOverlap.AddDynamic(this, &AKeyReward::OnOverlapStart);
	keyHitbox->OnComponentEndOverlap.AddDynamic(this, &AKeyReward::OnOverlapEnd);


	if (KeyWidget) {

		KeyWidget->AddToViewport();
		KeyWidget->GetWidgetFromName("PickupPrompt")->SetVisibility(ESlateVisibility::Hidden);

	}

	FSoftClassPath interactClassRef(TEXT("/Game/_Main/UI/InteractionWidget.InteractionWidget_C"));

	if (UClass* widgetClass = interactClassRef.TryLoadClass<UUserWidget>()) {

		InteractComponent->SetWidgetClass(widgetClass);
		InteractWidget = InteractComponent->GetWidget();

	}

}

void AKeyReward::SetupInputComponent()
{

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	InputComponent->BindAction("Interact", IE_Pressed, this, &AKeyReward::pickUp);
	DisableInput(GetWorld()->GetFirstPlayerController());

}

void AKeyReward::pickUp()
{
	if (HitBoxPlayer)
	{
		HitBoxPlayer->KeyCount++;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%d"), HitBoxPlayer->KeyCount));
		updateWidget();
		Destroy();
	}
}

// Called every frame
void AKeyReward::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


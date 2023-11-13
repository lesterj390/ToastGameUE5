// Don't steal out game please :( we worked hard on it!


#include "Props/ItemShelf.h"

// Sets default values
AItemShelf::AItemShelf()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ShelfMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shelf Mesh"));
	ShelfMesh->SetupAttachment(RootComponent);

	ItemSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Item Spawn"));
	ItemSpawn->SetupAttachment(ShelfMesh);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);

	InteractComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Widget"));
	InteractComponent->SetupAttachment(ItemSpawn);

	canPickup = true;
}

// Called when the game starts or when spawned
void AItemShelf::BeginPlay()
{
	Super::BeginPlay();
	
	SetupShelf();
	GenerateItem();
}

void AItemShelf::SetupShelf()
{
	InteractComponent->SetVisibility(false);

	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AItemShelf::OnOverlapStart);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &AItemShelf::OnOverlapEnd);

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &AItemShelf::PickupItem);
}

void AItemShelf::GenerateItem()
{
	FVector Location = FVector(0);
	FRotator Rotation = FRotator(0);

	float spawnRarity = FMath::RandRange(0.0f, 1.0f);
	if (spawnRarity >= 0 && spawnRarity <= RareSpawnRate) {
		int itemIndex = FMath::RandRange(0, RareItems.Num() - 1);
		ChosenItem = GetWorld()->SpawnActor<AInventoryItem>(RareItems[itemIndex], Location, Rotation);
	}
	if (spawnRarity > RareSpawnRate && spawnRarity <= RareSpawnRate + UncommonSpawnRate) {
		int itemIndex = FMath::RandRange(0, UncommonItems.Num() - 1);
		ChosenItem = GetWorld()->SpawnActor<AInventoryItem>(UncommonItems[itemIndex], Location, Rotation);
	}
	if (spawnRarity > RareSpawnRate + UncommonSpawnRate && spawnRarity <= 1) {
		int itemIndex = FMath::RandRange(0, CommonItems.Num() - 1);
		ChosenItem = GetWorld()->SpawnActor<AInventoryItem>(CommonItems[itemIndex], Location, Rotation);
	}

	if (ChosenItem) {
		ChosenItem->AttachToComponent(ItemSpawn, FAttachmentTransformRules::KeepRelativeTransform);
		ChosenItem->SetActorRelativeLocation(ChosenItem->ShelfLocation);
		ChosenItem->SetActorRelativeRotation(ChosenItem->ShelfRotation);
		ChosenItem->SetActorScale3D(ChosenItem->ShelfScale);
		ChosenItem->SetActorHiddenInGame(false);
	}
}

void AItemShelf::PickupItem()
{
	canPickup = false;

	HitBoxPlayer->Inventory->AddItem(ChosenItem->GetClass());
	InteractComponent->DestroyComponent();
	ChosenItem->Destroy();

	DisableInput(GetWorld()->GetFirstPlayerController());
}

void AItemShelf::OnOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				if (canPickup) {
					EnableInput(GetWorld()->GetFirstPlayerController());
					InteractComponent->SetVisibility(true);

					HitBoxPlayer = Cast<ATBCCharacter>(FoundActors[0]);
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Found Permanent Actor Reference");
				}
			}
		}
	}
}

void AItemShelf::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HitBoxPlayer) {
		if (canPickup) {
			DisableInput(GetWorld()->GetFirstPlayerController());
			InteractComponent->SetVisibility(false);
		}
	}
}

// Called every frame
void AItemShelf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


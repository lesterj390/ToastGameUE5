// Don't steal out game please :( we worked hard on it!


#include "Inventory/InventoryItem.h"

AInventoryItem::AInventoryItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;

	// One an item is spawned for the first time, it should be unequiped and hidden
	Unequip();
}

void AInventoryItem::Use()
{
	if (ItemQuantity > 0) {
		ItemQuantity--;
	}
}

void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	// Any specific code needed for item setup will go here
}

void AInventoryItem::Equip()
{
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);	
}

void AInventoryItem::Unequip()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}
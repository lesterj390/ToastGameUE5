// Don't steal out game please :( we worked hard on it!


#include "Inventory/InventoryItem.h"

AInventoryItem::AInventoryItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;

}

void AInventoryItem::Use()
{
	// Use function is different for each object
}

void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();

	// One an item is spawned for the first time, it should be unequiped and hidden
	Unequip();
}

void AInventoryItem::Equip()
{
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	bIsEquipped = true;
}

void AInventoryItem::Unequip()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsEquipped = false;
}
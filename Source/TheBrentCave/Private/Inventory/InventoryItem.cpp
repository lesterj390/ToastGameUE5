// Don't steal out game please :( we worked hard on it!


#include "Inventory/InventoryItem.h"

AInventoryItem::AInventoryItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}


void AInventoryItem::Setup()
{

}

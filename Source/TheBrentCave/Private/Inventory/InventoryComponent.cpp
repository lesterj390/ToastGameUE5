// Don't steal out game please :( we worked hard on it!


#include "Inventory/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SelectedItemIndex = 0;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::Setup(UStaticMeshComponent* ItemComponent)
{
	for (TSubclassOf<UInventoryItem> ItemClass : SpawnItems) {
		UInventoryItem* NewItem = NewObject<UInventoryItem>(this, ItemClass);
		Items.Add(NewItem);
	}

	if (Items.Num() == 0) return;
	UInventoryItem* SelectedItem = Items[SelectedItemIndex];
	if (!SelectedItem->Mesh) return;

	ItemComponent->SetStaticMesh(SelectedItem->Mesh);
	ItemComponent->SetRelativeScale3D(FVector(SelectedItem->MeshScale));	
}

void UInventoryComponent::AddItem(UClass* Class)
{
	UInventoryItem* NewItem = NewObject<UInventoryItem>(this, Class);
	Items.Add(NewItem);
}

void UInventoryComponent::NextItem()
{
}

void UInventoryComponent::PreviousItem()
{

}


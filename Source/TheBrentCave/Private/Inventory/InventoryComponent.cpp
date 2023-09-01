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

void UInventoryComponent::Setup(UChildActorComponent* ItemComponent)
{
	for (TSubclassOf<AInventoryItem> ItemClass : SpawnItems) {
		AddItem(ItemClass);
	}

	if (Items.Num() == 0) return;
	AInventoryItem* SelectedItem = Items[SelectedItemIndex];
	SelectedItem->Setup();
	ItemComponent->SetChildActorClass(SelectedItem->GetClass(), SelectedItem);
}

void UInventoryComponent::AddItem(UClass* Class)
{
	AInventoryItem* NewItem = NewObject<AInventoryItem>(this, Class);
	NewItem->Setup();
	Items.Add(NewItem);
}

void UInventoryComponent::NextItem()
{
}

void UInventoryComponent::PreviousItem()
{

}


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

void UInventoryComponent::Setup(USceneComponent* InItemComponent, ACharacter* PlayerP)
{
	if (!InItemComponent) return;

	Player = PlayerP;
	ItemComponent = InItemComponent;

	for (TSubclassOf<AInventoryItem> ItemClass : InitialItems) {
		AddItem(ItemClass);
	}
}

void UInventoryComponent::AddItem(UClass* Class)
{
	FVector Location = FVector(0);
	FRotator Rotation = FRotator(0);

	AInventoryItem* NewItem = GetWorld()->SpawnActor<AInventoryItem>(Class, Location, Rotation);
	NewItem->AttachToComponent(ItemComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Items.Add(NewItem);
}

FOnItemChange UInventoryComponent::NextItem()
{
	FOnItemChange itemDelegate;
	if (Items.Num() <= 1) return itemDelegate;

	int nextIndex = (SelectedItemIndex + 1) % Items.Num();

	GEngine->AddOnScreenDebugMessage(-1,  3.f, FColor::Cyan, FString::Printf(TEXT("Current Index Is: %d"), nextIndex));
	return SwitchToItem(nextIndex);
}

FOnItemChange UInventoryComponent::PreviousItem()
{
	FOnItemChange itemDelegate;
	if (Items.Num() <= 1) return itemDelegate;

	int nextIndex = (SelectedItemIndex + Items.Num() - 1) % Items.Num();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Current Index Is: %d"), nextIndex));
	
	return SwitchToItem(nextIndex);
}

void UInventoryComponent::RemoveItem(int itemIndex)
{
	AInventoryItem* CurrentItem = Items[itemIndex];
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Removing index: %d"), itemIndex));
	FOnItemChange itemDelegate = NextItem();
	itemDelegate.AddLambda([&]() {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Animation is done")));
		Items.RemoveAt(itemIndex);
	});
}

void UInventoryComponent::UseEquipedItem()
{
	AInventoryItem* CurrentItem = Items[SelectedItemIndex];
	CurrentItem->Use();

	if (CurrentItem->ItemQuantity == 0) {
		RemoveItem(SelectedItemIndex);
	}
}

FOnItemChange UInventoryComponent::SwitchToItem(int newItemIndex)
{
	UAnimMontage* newAnimation = Items[newItemIndex]->EquipAnimation;
	FOnItemChange itemChangeDelegate;
	if (CurrentAnimation) {
		UAnimInstance* currentInstance = Player->GetMesh()->GetAnimInstance();
		FOnMontageEnded montageEndedDelegate;
		montageEndedDelegate.BindLambda([&](UAnimMontage* montage, bool interrupted)
		{
			Items[SelectedItemIndex]->Unequip();
			Items[newItemIndex]->Equip();
			if (newAnimation) {
				Player->PlayAnimMontage(newAnimation);
			}
			SelectedItemIndex = newItemIndex;
			itemChangeDelegate.Broadcast();
		});
		currentInstance->Montage_SetEndDelegate(montageEndedDelegate, CurrentAnimation);		
		Player->StopAnimMontage();
		CurrentAnimation = newAnimation;
	}
	else {
		Items[newItemIndex]->Equip();
		if (newAnimation) {
			Player->PlayAnimMontage(newAnimation);
		}
		CurrentAnimation = newAnimation;
		SelectedItemIndex = newItemIndex;
		itemChangeDelegate.Broadcast();
	}
	return itemChangeDelegate;
}


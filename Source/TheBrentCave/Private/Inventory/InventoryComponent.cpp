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
	 //Getting player stats widget
	APlayerController* playerController = Cast<APlayerController>(Player->GetController());
	ATBC_HUD* playerHUD = Cast<ATBC_HUD>(playerController->MyHUD);
	PlayerStatsWidget = playerHUD->PlayerStatsWidget;
	
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

void UInventoryComponent::NextItem()
{
	if (Items.Num() <= 1) return;

	int nextIndex = (SelectedItemIndex + 1) % Items.Num();		
	SwitchToItem(nextIndex);
}

void UInventoryComponent::PreviousItem()
{	
	if (Items.Num() <= 1) return;

	int nextIndex = (SelectedItemIndex + Items.Num() - 1) % Items.Num();
	SwitchToItem(nextIndex);
}

void UInventoryComponent::RemoveItem(int itemIndex)
{
	if (!Items.IsValidIndex(itemIndex)) return;
	AInventoryItem* currentItem = Items[itemIndex];	
	NextItem();
	// Wait for item unequip animation to play before destroying item
	onItemUnequiped.AddWeakLambda(this, [this, itemIndex, currentItem] () {
		Items.RemoveAt(itemIndex);
		currentItem->Destroy();
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

void UInventoryComponent::SwitchToItem(int newItemIndex)
{
	if (!Items.IsValidIndex(newItemIndex) || !Items.IsValidIndex(SelectedItemIndex)) return;
	AInventoryItem* newItem = Items[newItemIndex];
	AInventoryItem* currentItem = Items[SelectedItemIndex];
	if (!newItem || !currentItem) return;
	UAnimMontage* newAnimation = newItem->EquipAnimation;
	UTexture2D* newIcon = newItem->Icon;	

	onItemUnequiped.RemoveAll(this);

	if (CurrentAnimation) {
		UAnimInstance* currentInstance = Player->GetMesh()->GetAnimInstance();
		FOnMontageEnded montageEndedDelegate;
		// This lambda function executes when the unequip animation finishes playing / the equip animation blends out
		montageEndedDelegate.BindLambda([=](UAnimMontage* montage, bool interrupted)
		{
			currentItem->Unequip();
			onItemUnequiped.Broadcast();
			newItem->Equip();			
			if (newAnimation) {
				Player->PlayAnimMontage(newAnimation);
			}
			SelectedItemIndex = newItemIndex;
		});
		
		// Binds the delegate to the animation instances montage end delegate
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
	}
	if (!newIcon) newIcon = DefaultIcon;
	PlayerStatsWidget->SelectedItem->SetBrushFromTexture(newIcon);			
}


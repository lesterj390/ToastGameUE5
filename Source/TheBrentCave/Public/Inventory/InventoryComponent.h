// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/Map/MapItem.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/Character.h"
#include "UI/PlayerStatsWidget.h"
#include "UI/TBC_HUD.h"
#include "InventoryComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnItemUnequip);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRENTCAVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AInventoryItem>> InitialItems;

	UPROPERTY()
	TArray<AInventoryItem*> Items;

	UPROPERTY()
	UAnimMontage* CurrentAnimation;

	UPROPERTY(EditAnywhere, Category = "UI")
	UTexture2D* DefaultIcon;

protected:
	UPROPERTY()
	int SelectedItemIndex;

	UPROPERTY()
	USceneComponent* ItemComponent;

	UPROPERTY()
	ACharacter* Player;

	UPROPERTY()
	UPlayerStatsWidget* PlayerStatsWidget;

	FOnItemUnequip onItemUnequiped;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Setup(USceneComponent* InItemComponent, ACharacter* PlayerP);

	UFUNCTION()
	void AddItem(UClass* Class);
	
	void NextItem();
	
	void PreviousItem();

	UFUNCTION()
	void RemoveItem(int itemIndex);

	UFUNCTION()
	void UseEquipedItem();
	
	void SwitchToItem(int newItemIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};

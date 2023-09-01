// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/Map/MapItem.h"
#include "Components/ChildActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEBRENTCAVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<AInventoryItem>> SpawnItems;

	UPROPERTY()
		TArray<AInventoryItem*> Items;

	UPROPERTY()
		int SelectedItemIndex;

protected:


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void Setup(UChildActorComponent* ItemComponent);

	UFUNCTION()
		void AddItem(UClass* Class);

	UFUNCTION()
		void NextItem();

	UFUNCTION()
		void PreviousItem();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};

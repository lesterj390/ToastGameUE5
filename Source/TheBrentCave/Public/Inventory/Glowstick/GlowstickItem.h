// Don't steal our game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Glowstick.h"
#include "GlowstickItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API AGlowstickItem : public AInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> GlowstickActor;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* DynamicMaterial;

protected:
	UPROPERTY()
	FLinearColor CurrentColor;

public:
	AGlowstickItem();

	virtual void Use() override;

protected:
	virtual void BeginPlay() override;

	FLinearColor SetRandomGlowstickColour();

};

// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/DecalComponent.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API AInventoryItem : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		UAnimSequence* HoldingAnimation;

protected:


public:
	AInventoryItem();

	virtual void Setup();
protected:

};

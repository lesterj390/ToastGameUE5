// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class THEBRENTCAVE_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UInventoryItem();

	UPROPERTY(EditAnywhere)
		TSubclassOf<UAnimSequence> HoldingAnimation;

	UPROPERTY(EditAnywhere)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
		float MeshScale;

	UPROPERTY(EditAnywhere)
		FRotator MeshRotation;


protected:

public:

protected:

};

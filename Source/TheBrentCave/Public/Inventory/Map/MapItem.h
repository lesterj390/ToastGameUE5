// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MapItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UMapItem : public UInventoryItem
{
	GENERATED_BODY()
	
public:
	UMapItem();

	UTextureRenderTarget2D* RenderTarget;
protected:

public:

protected:

};

// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Inventory/Map/MapWidget.h"
#include "Grid.h"
#include "Slate/WidgetRenderer.h"
#include "MapItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API AMapItem : public AInventoryItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UMapWidget> MapWidgetClass;

	UPROPERTY(EditAnywhere)
		UDecalComponent* Decal;

protected:
	UPROPERTY()
		UMapWidget* MapWidget;

	UPROPERTY()
		UTextureRenderTarget2D* RenderTarget;

	UPROPERTY()
		UMaterialInstanceDynamic* MapMaterialDynamic;

public:
	AMapItem();

	virtual void Setup() override;

protected:
};

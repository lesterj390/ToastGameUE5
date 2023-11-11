// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Inventory/Map/MapWidget.h"
#include "Grid.h"
#include "Slate/WidgetRenderer.h"
#include "Components/WidgetComponent.h"
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

	UPROPERTY()
		UMapWidget* MapWidget;

	UPROPERTY(EditAnywhere)
		UTextureRenderTarget2D* MapRenderTarget;

	FWidgetRenderer* WidgetRenderer;

	TSharedPtr<SWidget> MapSlateWidget;

	UPROPERTY()
		FVector2D DrawSize;

	UPROPERTY()
		UMaterialInstanceDynamic* MapMaterialDynamic;

protected:

public:
	AMapItem();

	void UpdateMapMaterial();

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
};

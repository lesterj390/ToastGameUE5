// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Puzzles/Maze/WallWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Maze/DataTypes.h"
#include "Components/Border.h"
#include "Components/RetainerBox.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanelSlot.h"
#include "Grid.h"
#include "MapWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	int Dimension;

	float WidgetResolution;

	float CellSize;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UWallWidget> WallClass;

	UPROPERTY(EditAnywhere)
		float VisibleDiameter;

	UPROPERTY()
		bool bCanTick;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		URetainerBox* RetainerBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UCanvasPanel* SizeCanvas;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UCanvasPanel* WallsCanvas;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UImage* PlayerIcon;

	FVector2D RenderScale;

public:
	UFUNCTION()
		void SetupWidget(int Dimension, float WidgetResolution, TArray<FWall> WallsToRemove);

protected:
	UFUNCTION()
		void SummonWall(FWall WallLocation, int Rotation);

	UFUNCTION()
		void GenerateWalls();

	UFUNCTION()
		void RemoveWalls(TArray<FWall> WallsToRemove);

	UFUNCTION()
		bool IsTranslationInBounds(float _TranslateX, float _TranslateY, float& MaxTranslation);

private:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);
};

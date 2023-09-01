// Don't steal out game please :( we worked hard on it!


#include "Inventory/Map/MapItem.h"

AMapItem::AMapItem()
{
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(Mesh);
}


void AMapItem::Setup()
{
	if (!MapWidgetClass) return;
	MapWidget = CreateWidget<UMapWidget>(GetWorld(), MapWidgetClass);

	AActor* FoundGrid = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	AGrid* Grid = Cast<AGrid>(FoundGrid);

	if (!Grid) return;

	MapWidget->SetupWidget(Grid->Dimensions, 1080, Grid->MazeAlgorithm->RemovedWalls);

	FVector2D DrawSize = FVector2D(1080);

	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
	RenderTarget = WidgetRenderer->CreateTargetFor(DrawSize, TF_Bilinear, false);
	TSharedRef<SWidget> SlateWidget = MapWidget->TakeWidget();
	WidgetRenderer->DrawWidget(SlateWidget, DrawSize);

	// Setting up dynamic material instance
	MapMaterialDynamic = Decal->CreateDynamicMaterialInstance();

	MapMaterialDynamic->SetTextureParameterValue(FName("Texture"), RenderTarget);
	Decal->SetDecalMaterial(MapMaterialDynamic);
}

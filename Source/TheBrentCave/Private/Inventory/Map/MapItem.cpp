// Don't steal out game please :( we worked hard on it!


#include "Inventory/Map/MapItem.h"

AMapItem::AMapItem()
{
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(Mesh);
}


void AMapItem::BeginPlay()
{
	Super::BeginPlay();

	if (!MapWidgetClass) return;
	MapWidget = CreateWidget<UMapWidget>(GetWorld(), MapWidgetClass);

	AActor* FoundGrid = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	AGrid* Grid = Cast<AGrid>(FoundGrid);

	if (!Grid) return;

	MapWidget->SetupWidget(Grid->Dimensions, 1080, Grid->MazeAlgorithm->RemovedWalls);
	
	MapSlateWidget = TSharedPtr<SWidget>(MapWidget->TakeWidget());
	DrawSize = FVector2D(1080);
	WidgetRenderer = new FWidgetRenderer(false);

	// Setting up dynamic material instance
	MapMaterialDynamic = Decal->CreateDynamicMaterialInstance();
}

void AMapItem::UpdateMapMaterial()
{
	WidgetRenderer->DrawWidget(MapRenderTarget, MapSlateWidget.ToSharedRef(), DrawSize, 0, false);
	MapMaterialDynamic->SetTextureParameterValue(FName("Texture"), MapRenderTarget);
}

void AMapItem::Tick(float DeltaTime)
{
	UpdateMapMaterial();
}

void AMapItem::BeginDestroy()
{
	Super::BeginDestroy();
	if (WidgetRenderer) delete WidgetRenderer;
	MapSlateWidget.Reset();
}

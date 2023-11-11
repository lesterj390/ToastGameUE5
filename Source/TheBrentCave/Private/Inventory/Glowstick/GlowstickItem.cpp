// Don't steal our game please :( we worked hard on it!


#include "Inventory/Glowstick/GlowstickItem.h"

AGlowstickItem::AGlowstickItem()
{
	//Component = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Component"));
	//Component->SetupAttachment(RootComponent);

	//Mesh->SetupAttachment(Component);	
}

void AGlowstickItem::Use()
{
	if (ItemQuantity == 0) return;

	AGlowstick* spawnedGlowstick = GetWorld()->SpawnActor<AGlowstick>(GlowstickActor, GetActorLocation(), GetActorRotation());
	spawnedGlowstick->SetGlowstickColour(CurrentColor);
	CurrentColor = SetRandomGlowstickColour();

	Super::Use();
}

void AGlowstickItem::BeginPlay()
{
	Super::BeginPlay();
	CurrentColor = SetRandomGlowstickColour();
}

FLinearColor AGlowstickItem::SetRandomGlowstickColour()
{
	FLinearColor newColour = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));

	if (!DynamicMaterial) {
		UMaterialInterface* staticMaterial = Mesh->GetMaterial(0);
		DynamicMaterial = UMaterialInstanceDynamic::Create(staticMaterial, this);
		Mesh->SetMaterial(0, DynamicMaterial);
	}

	DynamicMaterial->SetVectorParameterValue(TEXT("GlowstickColor"), newColour);
	DynamicMaterial->SetScalarParameterValue(TEXT("Glow"), 50);

	return newColour;
}



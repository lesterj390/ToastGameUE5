// Don't steal out game please :( we worked hard on it!


#include "Inventory/Radar/RadarItem.h"

ARadarItem::ARadarItem()
{
	RadarOn = false;
}

void ARadarItem::BeginPlay()
{
	Super::BeginPlay();

}

void ARadarItem::Tick(float DeltaTime)
{
	
}

void ARadarItem::Use() {
	RadarOn = !RadarOn;

	if (RadarOn) {
		UpdateRadarDistance();
	}
}

void ARadarItem::Unequip()
{
	Super::Unequip();

	RadarOn = false;
}

void ARadarItem::UpdateRadarDistance()
{

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToastClass, FoundActors);
	if (FoundActors.Num() > 0) {
		ToastRef = FoundActors[0];

		if (GetDistanceTo(ToastRef) < 10000 && RadarOn) {

			float DistanceToToast = GetDistanceTo(ToastRef) / 10000;

			RadarDistance = FMath::Lerp(0.1, 2, DistanceToToast);
			RadarPitch = FMath::Lerp(1.5, 0.2, DistanceToToast);

			UAudioComponent* RadarComponent = UGameplayStatics::CreateSound2D(GetWorld(), RadarSound);
			RadarComponent->SetPitchMultiplier(RadarPitch);

			RadarComponent->Play(0.0);

			GetWorldTimerManager().ClearTimer(RadarSignalTimer);
			GetWorldTimerManager().SetTimer(RadarSignalTimer, this, &ARadarItem::UpdateRadarDistance, RadarDistance, false);
		}
		else if (GetDistanceTo(ToastRef) > 10000 && RadarOn) {

			GetWorldTimerManager().ClearTimer(RadarSignalTimer);
			GetWorldTimerManager().SetTimer(RadarSignalTimer, this, &ARadarItem::UpdateRadarDistance, 0.5, false);

		}
		else {
			RadarOn = false;
		}
	}
}
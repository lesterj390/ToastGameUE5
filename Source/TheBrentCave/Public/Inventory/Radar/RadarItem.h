// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "RadarItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API ARadarItem : public AInventoryItem
{
	GENERATED_BODY()
	


		// Functions
public:
	ARadarItem();

	virtual void Use() override;
	virtual void Unequip() override;

protected:
	void UpdateRadarDistance();

	// Variables
public:

	UPROPERTY(EditAnywhere)
		float RadarUpdateRate;

	float RadarDistance;
	float RadarPitch;

	FTimerHandle RadarSignalTimer;

	UPROPERTY(EditAnywhere)
		USoundCue* RadarSound;

	bool RadarOn;

	// Toast
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ToastClass;

	UPROPERTY()
		AActor* ToastRef;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
};
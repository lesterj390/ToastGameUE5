// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockers/Locker.h"
#include "Coffin.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API ACoffin : public ALocker
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere)
		UCurveFloat* PeakCurve;

	UPROPERTY(EditAnywhere, Category = "DoorAnimation");
		float OpenPeakAngle;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<UUserWidget> PeakWidgetClass;

protected:
	bool bIsPeaking;

	FTimeline PeakTimeline;

	FTimerHandle PeakHandle;

	UUserWidget* PeakWidget;

public:
	virtual void EnterLocker() override;

	virtual void LeaveLocker() override;

protected:
	UFUNCTION()
		void UpdateTimelineProgressPeak(float Value);

	void SetupPeakCurve();

	void StartPeak();

	void EndPeak();

	UFUNCTION()
		void EnableCollision();

	UFUNCTION()
		void EnablePeak();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};

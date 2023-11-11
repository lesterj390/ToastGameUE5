// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Glowstick.generated.h"

UCLASS()
class THEBRENTCAVE_API AGlowstick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlowstick();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMaterialInterface* tempMat;

	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic* glowstickMat;

	UPROPERTY()
		FLinearColor glowColor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* CapsuleCollision;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Glowstick;

	UPROPERTY(VisibleAnywhere)
		UPointLightComponent* stickLight;

	UFUNCTION()
		void UpdateTimelineProgress(float Value);

	UFUNCTION()
		void SetupFadeCurve();

	UFUNCTION()
		void SetGlowstickColour(FLinearColor newColour);

	UPROPERTY(EditAnywhere)
		UCurveFloat* fadeCurve;

	UFUNCTION()
		void FinishTimeline();

	FTimeline fadeTimeline;

	bool setupMat;

};
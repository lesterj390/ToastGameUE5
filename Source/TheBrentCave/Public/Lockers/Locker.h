// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "TBCCharacter.h"
#include "Locker.generated.h"


UCLASS()
class THEBRENTCAVE_API ALocker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALocker();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LockerBase;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LockerDoor;

	UPROPERTY(EditAnywhere)
		UCurveFloat* DoorCurve;

	UPROPERTY(EditAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere)
		UCameraComponent* LockerCamera;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;
	
	UPROPERTY(EditAnywhere, Category="DoorAnimation");
		float ClosedDoorAngle;

	UPROPERTY(EditAnywhere, Category="DoorAnimation");
		float OpenDoorAngle;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* OuterLockerLight;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* InnerLockerLight;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY()
		UUserWidget* InteractWidget;
		


	//UFUNCTION()
	//	void ControlDoor();

	//UFUNCTION()
	//	void ToggleDoor();

	//UFUNCTION()
	//	void SetState();

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void EnterLocker();

	virtual void LeaveLocker();

	virtual void InteractLocker();


	UFUNCTION()
		void UpdateTimelineProgress(float Value);

	UFUNCTION()
		void AddCameraYaw(float Val);

	UFUNCTION()
		void AddCameraPitch(float Val);

		bool IsHiding;

	//bool Open;
	//bool ReadyState;
	//float CurveFloatValue;
	//float TimelineValue;
	//FRotator DoorRotation;
	FTimeline DoorTimeline;
	float Pitch;
	float Yaw;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetupInputComponent();
	void SetupDoorCurve();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
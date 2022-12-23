// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "TBCCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "KeyReward.generated.h"

UCLASS()
class THEBRENTCAVE_API AKeyReward : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyReward();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* keyMesh;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* keyHitbox;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> WidgetSubclass;

	UPROPERTY()
		UUserWidget* KeyWidget;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* SmokeBurst;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void updateWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void SetupInputComponent();
	void pickUp();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/BoxComponent.h"
#include "Grid.h"
#include "TBCCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DoorKey.generated.h"

UCLASS()
class THEBRENTCAVE_API ADoorKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorKey();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DoorKeyMesh;

	UPROPERTY()
		UUserWidget* doorKeyWidget;

	UPROPERTY(VisibleAnywhere)
		UWidgetComponent* keysComponent;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY()
		AGrid* grid;

	UPROPERTY()
		ATBCCharacter* Player;

	UPROPERTY()
		UUserWidget* UseWidget;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* UseWidgetComponent;

	UPROPERTY()
		UUserWidget* EquipKeysWidget;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* EquipKeysComponent;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> DoorKeyBreak;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	int puzzleAm;

	bool inOverlap;

	UFUNCTION()
		void insertKey();

	void updateWidget();
	void setupInput();

};

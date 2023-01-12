// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
//#include "AIController.h"
#include "Components/BoxComponent.h"
#include "TBCCharacter.h"
#include "NumberPuzzle.generated.h"

UCLASS()
class THEBRENTCAVE_API ANumberPuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANumberPuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void randomShapeSetup();

	int shapes[16];

	int shapeVal[4];

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int answer;

	UPROPERTY(EditAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* numberBase;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* PuzzleLight;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* ShapeComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* VertComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* HoriComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* NumberInputComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* HintComponent;

	UPROPERTY()
		UUserWidget* ShapeWidget;

	UPROPERTY()
		UUserWidget* VertWidget;

	UPROPERTY()
		UUserWidget* HoriWidget;

	UPROPERTY()
		UUserWidget* InputWidget;

	UPROPERTY()
		UUserWidget* PromptWidget;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UPROPERTY()
		UUserWidget* HintWidget;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool inOverlap;

	void setupInput();

	void enterPuzzle();

	UFUNCTION(BlueprintCallable, Category = "ExitPuzzle")
	void exitPuzzle();

	void GetHint();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool inPuzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString exitString;

};

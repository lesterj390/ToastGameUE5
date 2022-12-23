// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
//#include "UObject/ConstructorHelpers.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Math/UnrealMathUtility.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "TBCCharacter.h"
#include "Components/SpotLightComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "KeyFrame.generated.h"


UCLASS()
class THEBRENTCAVE_API AKeyFrame : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyFrame();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* WidgetPivot;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* WidgetComponent;

	UPROPERTY()
		UUserWidget* KeyWidget;

	TArray<char> alphabet;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere)
		UCameraComponent* KeyFrameCamera;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* PuzzleLight;

	bool InPuzzle;

	UPROPERTY()
		FKey currentKey;

	//UPROPERTY()
		FInputKeyBinding currentKeyBinding;

	UPROPERTY(EditAnywhere)
		int NumberOfKeys;

	int RemainingKeys;

	UPROPERTY(EditAnywhere)
		float TimePerKey;

	float RemainingTime;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
		int CountdownSeconds;

	int RemainingCountdown;

	FString interactString;

	FString exitString;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;


	UFUNCTION()
		void Interact();

	UFUNCTION()
		void Exit();

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
		void DisplayNewKey();

	UFUNCTION()
		void KeyPress();

	void FinishPuzzle();

	//UFUNCTION()
		void ClearKeyBindings(TArray<FInputKeyBinding> * KeyBindings);

	UFUNCTION()
		void TimerFunction();

	UFUNCTION()
		void BindIncorrectKeys();

	UFUNCTION()
		void CountDown();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> breakKeyFrameActor;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetupInputComponent();

	char alphabetStatic[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

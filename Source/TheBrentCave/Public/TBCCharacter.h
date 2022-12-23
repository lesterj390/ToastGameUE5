// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
//#include "Locker.h"
#include "Grid.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Glowstick.h"
#include "Math/UnrealMathVectorCommon.h"
#include "UI/TBC_HUD.h"
#include "Engine/EngineTypes.h"
#include "TBCCharacter.generated.h"

UCLASS(config=Game)
class ATBCCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

	
public:
	ATBCCharacter();

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	//UPROPERTY(EditAnywhere)
	//	class ALocker* CurrentDoor;

	UPROPERTY()
		TSubclassOf<UUserWidget> InteractWidgetSubclass;

	UPROPERTY()
		UUserWidget* InteractWidget;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
		float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
		float SprintSpeed;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float StaminaRegenerationRate;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float StaminaConsumptionRate;
		
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float StaminaCooldown;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float UpdateRate;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float SanityDuration;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float SanityRegenerationDuration;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float MaxBatteryCharge;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float BatteryReductionRate;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float BatteryReductionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
		float MaxRadarBattery;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float RadarUpdateRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int FoodAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int BatteryAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int MaxGlowstickAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int MaxFoodAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int MaxBatteryAmount;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool hasRadar;

	UPROPERTY(EditAnywhere, Category = "Lighting")
		float LightFlickerMin;

	UPROPERTY(EditAnywhere, Category = "Lighting")
		float LightFlickerMax;

	float defaultLightFlickerMin;
	float defaultLightFlickerMax;

	UPROPERTY(EditAnywhere, Category = "Lighting")
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Camera")
		TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY()
		AGrid* grid;

	int PuzzleCount;

	UPROPERTY()
		int KeyCount;

	float DefaultIntensity;

	float rightVal;

	float BatteryPower;

	FTimerHandle BatteryDrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InventroyProgressBar;

	void ReduceBattery();
	void ReduceRadarBattery();

	void BatteryInFlashlight();
	void BatteryInRadar();

	void UpdateInvBar();

	UPROPERTY()
		USoundCue* WhisperSound;

	UPROPERTY()
		UAudioComponent* StaminaAudioComponent;

	UPROPERTY()
		USoundCue* BackgroundSound;

	UPROPERTY(EditAnywhere)
		USceneComponent* SoundPivot;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> BreadcrumbActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> GlowstickActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> ExitActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> ExitDoorActor;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AActor> EatCrumbActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isHiding;

	UFUNCTION()
		void RemoveExitWall();

	UPROPERTY(EditAnywhere)
		USceneComponent* BreadcrumbMount;

	int selectedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool pickedUpItem;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ToastClass;

	UPROPERTY()
		AActor* ToastRef;

	void UpdateRadarDistance();

	UPROPERTY()
		USoundCue* RadarSound;

	float RadarDistance;
	float RadarPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RadarBattery;

	FTimerHandle RaderSignalTimer;
	FTimerHandle RadarBatteryDrain;

	bool RadarOn;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	bool bIsSprinting;

	bool bStartedSprint;

	void StartSprint();

	void EndSprint();

	class UPlayerStatsComponent* PlayerStats;

	FTimerHandle CheckForCooldownTimer;

	void CheckForCooldown();

	//Setup flashlight info
	void flashlightToggle();

	UPROPERTY()
		USoundCue* FlashlightToggleSound;

	//void lightFlicker();

	//void flickerReset();

	UFUNCTION()
		void displayFlicker(int flickerCount);

	bool flashlightOn;

	FTimerHandle flickerHandle;

	void StartRelax();

	void EndRelax();

	bool bIsRelaxing;

	bool bStartedRelax;

	FVector CalculateWhisperLocation(float angle);

	void InsanityWhisper(int multiplier);

	FTimerHandle sanityHandle;

	void SanityCheck();

	void StaminaCheck();

	int LastStaminaState;

	void SummonBreadcrumb();

	FVector MazeCenter;

	void throwGlowstick();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int GlowstickAmount;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* glowstickThrowPoint;

	//UPROPERTY(EditAnywhere)
	//	UStaticMeshComponent* FlashlightCone;

	UPROPERTY()
		AActor* throwReturn;

	UPROPERTY()
		AActor* EatCrumbReturn;

	UPROPERTY()
		FVector glowstickDirection;

	UFUNCTION()
		void InsanityCameraShake(float scale = 1.0f);

	FTimerHandle cameraShakeHandle;

	UCameraShakeBase* CameraShakeInstance;

	UFUNCTION()
		void UseItem();

	//void ChooseItemUp();
	//void ChooseItemDown();
	void ScrolledUp();
	void ScrolledDown();

	void Wait(float seconds);

protected:
	// APawn interface
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual void AddControllerYawInput(float Val) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }



};


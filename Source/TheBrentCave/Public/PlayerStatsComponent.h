// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "PlayerStatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEBRENTCAVE_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatsComponent();


	void ConsumeStamina();
	

	void RegenerateStamina();


	void SetRegenerationRate(float Rate);


	void SetConsumptionRate(float Rate);


	void SetCooldown(float Value);


	void SetUpdateRate(float Rate);


	bool CanSprint();


	void SetSanityDuration(float Value);


	void SetSanityRegenerationDuration(float Value);


	void ConsumeSanity();


	void RegenerateSanity();


	void IdleSanity();


	float GetSanity();

	int GetStaminaState();

	//void ClearTimer();

public:
	const int IDLE = 0;
	const int REGENERATING = 1;
	const int CONSUMING = 2;
	const int COOLDOWN = 3;

protected:

	float Stamina;

	int staminaState;

	float staminaRegenerationVPS;

	float staminaConsumingVPS;

	float cooldown;

	float updateRate;

	FTimerHandle StaminaHandle;

	float sanityDuration;

	float Sanity;

	float sanityRegenerationDuration;

	float sanityConsumingVPS;

	float sanityRegenerationVPS;

	int sanityState;

	FTimerHandle SanityHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	void CooldownStamina();

public:	
	

		
};

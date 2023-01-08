// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStatsComponent.h"
#include "UI/TBC_HUD.h"


// Sets default values for this component's properties
UPlayerStatsComponent::UPlayerStatsComponent()
{
	//Sprint/Stamina
	Stamina = 100.0f;
	staminaState = IDLE;

	staminaRegenerationVPS = 0.0f;
	staminaConsumingVPS = 0.0f;
	cooldown = 0.0f;
	sanityDuration = 0.0f;

	//Santiy
	sanityState = IDLE;
}

// Called when the game starts
void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Setting Stamina Bar to 100%
	if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)) {
		GameHUD->UpdateStaminaPercent(Stamina);
	}

	// Setting Sanity Bar to 100%
	if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD))
	{
		GameHUD->UpdateSanityPercent(100.0f);
	}

}


bool UPlayerStatsComponent::CanSprint() 
{
	if (staminaState != COOLDOWN && Stamina > 0)
	{
		return true;
	}
	else {
		return false;
	}
}


void UPlayerStatsComponent::SetRegenerationRate(float Rate)
{
	staminaRegenerationVPS = Rate;
}


void UPlayerStatsComponent::RegenerateStamina()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Reg Stamina: %f"), Stamina));
	if (staminaState == CONSUMING) {
		GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
		GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::RegenerateStamina, updateRate, true);
		staminaState = REGENERATING;
	}

	if (staminaState != COOLDOWN && GetSanity() > 10) {
		if ((Stamina + staminaRegenerationVPS) < 100) {
			//staminaState = REGENERATING;
			/*GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
			GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::CooldownStamina, cooldown, false, cooldown);*/
			Stamina += staminaRegenerationVPS;
		}
		else if (Stamina < 100) {
			Stamina = 100;
		}
		else {
			staminaState = IDLE;
			GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
		}
	}

	if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)) {
		GameHUD->UpdateStaminaPercent(Stamina);
	}
	
}


void UPlayerStatsComponent::SetConsumptionRate(float Rate)
{
	staminaConsumingVPS = Rate;
}


void UPlayerStatsComponent::ConsumeStamina()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Current Stamina: %f"), Stamina));
	if (staminaState == IDLE || staminaState == REGENERATING)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
		GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::ConsumeStamina, updateRate, true);
		staminaState = CONSUMING;
	}


	if (staminaState != COOLDOWN)
	{
		if (Stamina > staminaConsumingVPS)
		{
			Stamina -= staminaConsumingVPS;
		}
		else if (Stamina > 0)
		{
			Stamina = 0;
		}
		else {
			//staminaState = IDLE;
			GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
			staminaState = COOLDOWN;
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Cooldown Started")));
			GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::CooldownStamina, cooldown, false, cooldown);
		}

		//Updating HUD
		if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)) {
			GameHUD->UpdateStaminaPercent(Stamina);
		}
	}
}


void UPlayerStatsComponent::SetCooldown(float Value)
{
	cooldown = Value;
}


void UPlayerStatsComponent::SetUpdateRate(float Rate)
{
	updateRate = Rate;
	staminaConsumingVPS *= updateRate;
	staminaRegenerationVPS *= updateRate;
	sanityConsumingVPS = updateRate;
}


void UPlayerStatsComponent::CooldownStamina()
{
	staminaState = REGENERATING;
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Cooldown Over")));
	GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::RegenerateStamina, updateRate, true);
}


void UPlayerStatsComponent::SetSanityDuration(float Value)
{
	sanityDuration = Value;
	Sanity = sanityDuration;
}


void UPlayerStatsComponent::SetSanityRegenerationDuration(float Value)
{
	sanityRegenerationDuration = Value;
	sanityRegenerationVPS = sanityDuration / sanityRegenerationDuration * updateRate;
}


void UPlayerStatsComponent::ConsumeSanity()
{
	if (sanityState == IDLE || sanityState == REGENERATING)
	{

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity Consuming: %f"), Sanity));
		GetWorld()->GetTimerManager().ClearTimer(SanityHandle);
		GetWorld()->GetTimerManager().SetTimer(SanityHandle, this, &UPlayerStatsComponent::ConsumeSanity, updateRate, true);
		sanityState = CONSUMING;
	}

	
	if (Sanity > sanityConsumingVPS)
	{
		Sanity -= sanityConsumingVPS;
	}
	else if (Sanity > 0)
	{
		Sanity = 0;
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(SanityHandle);
		sanityState = IDLE;
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Gone Insane")));
	}

	//Updating HUD
	if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)) {
		GameHUD->UpdateSanityPercent(Sanity / sanityDuration * 100);
	}
	
}


void UPlayerStatsComponent::RegenerateSanity()
{
	if (sanityState == CONSUMING || sanityState == IDLE) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity Regenerating: %f"), Sanity));
		GetWorld()->GetTimerManager().ClearTimer(SanityHandle);
		GetWorld()->GetTimerManager().SetTimer(SanityHandle, this, &UPlayerStatsComponent::RegenerateSanity, updateRate, true);
		sanityState = REGENERATING;
	}

	
	if ((Sanity + sanityRegenerationVPS) < sanityDuration) {
		//staminaState = REGENERATING;
		/*GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
		GetWorld()->GetTimerManager().SetTimer(StaminaHandle, this, &UPlayerStatsComponent::CooldownStamina, cooldown, false, cooldown);*/
		Sanity += sanityRegenerationVPS;
	}
	else if (Sanity < sanityDuration) {
		Sanity = sanityDuration;
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(SanityHandle);
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Completely Sane")));
		//ConsumeSanity();
	}
	

	if (ATBC_HUD* GameHUD = Cast<ATBC_HUD>(GetWorld()->GetFirstPlayerController()->MyHUD)) {
		GameHUD->UpdateSanityPercent(Sanity / sanityDuration * 100);
	}
}


void UPlayerStatsComponent::IdleSanity()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Sanity Idle: %f"), Sanity));
	if (sanityState == REGENERATING || sanityState == CONSUMING)
	{
		GetWorld()->GetTimerManager().ClearTimer(SanityHandle);
		sanityState = IDLE;
	}
}


float UPlayerStatsComponent::GetSanity()
{
	return Sanity / sanityDuration * 100;
}

int UPlayerStatsComponent::GetStaminaState()
{
	return staminaState;
}









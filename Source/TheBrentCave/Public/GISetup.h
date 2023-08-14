// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GISetup.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UGISetup : public UGameInstance
{
	GENERATED_BODY()

public:
		UGISetup();

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TArray<int> whichPuzzles;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			int PuzzleAmount;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			int GridSize;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			float HidingSpawnRate;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			int Map;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			AActor* Walls;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			AActor* Floor;

		//Ceilings
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TSubclassOf<AActor> I;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TSubclassOf<AActor> U;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TSubclassOf<AActor> X;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TSubclassOf<AActor> L;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			TSubclassOf<AActor> T;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			bool TestingGame;

		// Settings
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			float PlayerFOV;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			float PuzzleFOV;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float MasterVolume;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float AmbienceMusicVolume;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float SoundEffectsVolume;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float VoicesVolume;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float WhispersVolume;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Sensitivity;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool VInvert;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool HInvert;

		// Other

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool DisplaySuggestions;

protected:

};

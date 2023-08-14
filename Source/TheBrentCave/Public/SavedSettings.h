// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SavedSettings.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API USavedSettings : public USaveGame
{
	GENERATED_BODY()

public:
	USavedSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayerFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool DisplaySuggestions;
};

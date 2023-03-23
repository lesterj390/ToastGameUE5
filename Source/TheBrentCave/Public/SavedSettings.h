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
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheBrentCaveGameMode.h"
#include "TheBrentCaveHUD.h"
#include "UI/TBC_HUD.h"
#include "TheBrentCaveCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ATheBrentCaveGameMode::ATheBrentCaveGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	


	// use our custom HUD class
	HUDClass = ATBC_HUD::StaticClass();
}

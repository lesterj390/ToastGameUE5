// Fill out your copyright notice in the Description page of Project Settings.


#include "GISetup.h"

// Sets default values
UGISetup::UGISetup() {

	GridSize = 2;
	HidingSpawnRate = 0.0;
	TestingGame = true;
	Map = 0;

	PlayerFOV = 90;
	PuzzleFOV = 90;
	MasterVolume = 1;
	AmbienceMusicVolume = 1;
	SoundEffectsVolume = 1;
	VoicesVolume = 1;
	WhispersVolume = 1;
	Sensitivity = 0.07;
	VInvert = false;
	HInvert = false;
}
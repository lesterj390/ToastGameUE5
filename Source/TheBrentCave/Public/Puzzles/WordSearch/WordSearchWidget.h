// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"
#include "Engine/Font.h"
#include "Fonts/SlateFontInfo.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/BorderSlot.h"
#include "Misc/FileHelper.h"
#include "Blueprint/UserWidget.h"
#include "Math/UnrealMathUtility.h"
#include "WordSearchWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UWordSearchWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//UPROPERTY()
		//AWordSearch* PuzzleRef;

	UPROPERTY()
		UFont* GameFont;

	int WidgetRes;
	int MinPuzzleSize;
	int MaxPuzzleSize;
	int PuzzleSize;

	float spacing;

	void SetupWidget(int WidgetResParam, int MinPuzzleSizeP, int MaxPuzzleSizeP);

	char alphabetStatic[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	TArray<char> alphabet;

	void SetWord(FString WordP);

	void BankWords();

	UPROPERTY()
		TArray<FString> WordBank;

	UPROPERTY()
		TArray<UTextBlock*> GridArray;

	UPROPERTY()
		TArray<UImage*> CheckImageArray;

	UPROPERTY()
		UCanvasPanelSlot* imageSlot;

	UPROPERTY(EditAnywhere)
		UTexture2D* SelectionTexture;

	FString word;

	int RowSpawn;
	int ColSpawn;
	int WordRotation;

	int NextRow;
	int NextCol;

	int PosRow;
	int PosCol;

	static enum Direction { D_UP, D_DOWN, D_LEFT, D_RIGHT };
	static enum Rotation { R_UP_DIAGONAL, R_STRAIGHT, R_DOWN_DIAGONAL, R_DOWN };

	void ClearImageArray();
	void PreviewSelection();
	Rotation selectedRotation;
	int CheckRow;
	int CheckCol;


protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;	

private:
	virtual void NativeConstruct() override;

};

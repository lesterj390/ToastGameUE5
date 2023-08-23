// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/WordSearch/WordSearchWidget.h"

void UWordSearchWidget::NativeConstruct()
{
	Super::NativeConstruct();

	selectedRotation = R_UP_DIAGONAL;

}

void UWordSearchWidget::SetupWidget(int WidgetResParam, int MinPuzzleSizeP, int MaxPuzzleSizeP)
{

	WidgetRes = WidgetResParam;
	MinPuzzleSize = MinPuzzleSizeP;
	MaxPuzzleSize = MaxPuzzleSizeP;

	PuzzleSize = FMath::RandRange(MinPuzzleSize, MaxPuzzleSize);
	spacing = (float)WidgetRes / PuzzleSize;
	//PuzzleSize = FMath::Square(PuzzleSize);

	for (int i = 0; i < 26; i++) {

		if (alphabetStatic) {
			alphabet.Add(alphabetStatic[i]);
		}

	}

	if (WidgetTree) {

		UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
		WidgetTree->RootWidget = Canvas;
		Canvas->SetVisibility(ESlateVisibility::Visible);

		PosRow = 0;
		PosCol = 0;

		UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), (FName(*FString::Printf(TEXT("SelectedItem %d, %d"), PosRow, PosCol))));
		Canvas->AddChild(Image);

		//FString ImagePath = FString("/Game/_Main/Puzzles/WordSearch/Texture/SelectedWord.SelectedWord");
		//UTexture2D* SelectionTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *ImagePath));

		Image->SetBrushFromTexture(SelectionTexture);
	
		imageSlot = (UCanvasPanelSlot*)Image->Slot;
		if (imageSlot) {

			imageSlot->SetSize(FVector2D(spacing, spacing));
			imageSlot->SetPosition(FVector2D(0, 0));

		}

		for (int row = 0; row < PuzzleSize; row++) {

				for (int col = 0; col < PuzzleSize; col++) {
					UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), (FName(*FString::Printf(TEXT("Border %d, %d"), row, col))));
					Canvas->AddChild(Border);
					//Border->SetBrushColor(FColor(FColor::Cyan));
					Border->SetBrushColor(FLinearColor(0,0,0,0));
					Border->SetHorizontalAlignment(HAlign_Center);
					Border->SetVerticalAlignment(VAlign_Center);

					UTextBlock* Letter = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), (FName(*FString::Printf(TEXT("%d, %d"), row, col))));
					Border->AddChild(Letter);
					Letter->SetText(FText::FromString(FString::Printf(TEXT("%c"), alphabet[FMath::RandRange(0, alphabet.Num() - 1)])));
					//Letter->SetFont();

					UCanvasPanelSlot* borderSlot = (UCanvasPanelSlot*)Border->Slot;
					if (borderSlot) {
						borderSlot->SetSize(FVector2D(spacing, spacing));
						borderSlot->SetPosition(FVector2D(spacing * col, spacing * row));
						borderSlot->SetZOrder(1);
					}

					UBorderSlot* letterSlot = (UBorderSlot*)Letter->Slot;
					if (letterSlot) {
						FSlateFontInfo currentFont = Letter->GetFont();
						currentFont.Size = spacing - 18;
						currentFont.OutlineSettings.OutlineSize = 4;
						currentFont.OutlineSettings.OutlineColor.Black;
						Letter->SetFont(currentFont);
					}

				}
			}
	}
	RebuildWidget();

	BankWords();

}

void UWordSearchWidget::SetWord(FString WordP)
{
	word = WordP;
}

void UWordSearchWidget::BankWords()
{

	WordRotation = FMath::RandRange(0, 3);

	if (WordRotation == 0) {
		//Straight
		RowSpawn = FMath::RandRange(0, PuzzleSize - 1);
		ColSpawn = FMath::RandRange(0, (PuzzleSize - word.Len()) - 1);
	}
	else if (WordRotation == 1) {
		//Up Diagonal
		RowSpawn = FMath::RandRange(word.Len() - 1, PuzzleSize - 1);
		ColSpawn = FMath::RandRange(0, (PuzzleSize - word.Len()) - 1);
	}
	else if (WordRotation == 2) {
		//Down Diagonal
		RowSpawn = FMath::RandRange(0, (PuzzleSize - 1) - (word.Len() - 1));
		ColSpawn = FMath::RandRange(0, (PuzzleSize - word.Len()) - 1);
	}
	else if (WordRotation == 3) {
		//Down
		RowSpawn = FMath::RandRange(0, (PuzzleSize - 1) - (word.Len() - 1));
		ColSpawn = FMath::RandRange(0, PuzzleSize - 1);
	}

	//Rotations: 0 Straight, 1 Up Diagonal, 2 Down Diagonal, 3 Down

	NextRow = RowSpawn;
	NextCol = ColSpawn;

	for (int i = 0; i < word.Len(); i++) {

		UTextBlock* WordRef = Cast<UTextBlock>(WidgetTree->FindWidget(FName(FString::Printf(TEXT("%d, %d"), NextRow, NextCol))));
		WordRef->SetText(FText::FromString(FString::Printf(TEXT("%c"), word[i])));
		//WordRef->SetColorAndOpacity(FSlateColor(FColor::Red));

		if (WordRotation == 0) {
			NextCol++;
		}
		else if (WordRotation == 1) {
			NextRow--;
			NextCol++;
		}
		else if (WordRotation == 2) {
			NextRow++;
			NextCol++;
		}
		else if (WordRotation == 3) {
			NextRow++;
		}
			
	}

}

void UWordSearchWidget::ClearImageArray()
{

	if (CheckImageArray.Num() > 0) {
		for (int i = 0; i < CheckImageArray.Num(); i++) {
			WidgetTree->RemoveWidget(CheckImageArray[i]);
		}
	}

	selectedRotation = R_UP_DIAGONAL;


}

// Create widget elements to preview selected word
void UWordSearchWidget::PreviewSelection()
{

	//Rotations: 0 Straight, 1 Up Diagonal, 2 Down Diagonal, 3 Down

	CheckRow = RowSpawn;
	CheckCol = ColSpawn;

	UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
	WidgetTree->RootWidget = Canvas;

	if (CheckImageArray.Num() > 0) {
		for (int i = 0; i < CheckImageArray.Num(); i++) {
			WidgetTree->RemoveWidget(CheckImageArray[i]);
			//CheckImageArray.Remove(0);
		}
	}

	if (selectedRotation == R_UP_DIAGONAL) {
		for (int i = 0; i < word.Len() - 1; i++) {
			UImage* CheckImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), (FName(*FString::Printf(TEXT("Check %d, %d"), CheckRow, CheckCol))));
			Canvas->AddChild(CheckImage);
			CheckImageArray.Add(CheckImage);

			//FString CheckImagePath = FString("/Game/_Main/Puzzles/WordSearch/Texture/SelectedWord.SelectedWord");
			//UTexture2D* SelectionTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *CheckImagePath));

			CheckImage->SetBrushFromTexture(SelectionTexture);

			UCanvasPanelSlot* checkImageSlot = (UCanvasPanelSlot*)CheckImage->Slot;
			if (checkImageSlot) {

				checkImageSlot->SetSize(FVector2D(spacing, spacing));
				checkImageSlot->SetPosition(FVector2D(imageSlot->GetPosition().X + (spacing * (i + 1)), imageSlot->GetPosition().Y - (spacing * (i + 1))));
				CheckRow--;
				CheckCol++;

			}
		}
	}
	else if (selectedRotation == R_STRAIGHT) {
		for (int i = 0; i < word.Len() - 1; i++) {
			UImage* CheckImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), (FName(*FString::Printf(TEXT("Check %d, %d"), CheckRow, CheckCol))));
			Canvas->AddChild(CheckImage);
			CheckImageArray.Add(CheckImage);

			//FString CheckImagePath = FString("/Game/_Main/Puzzles/WordSearch/Texture/SelectedWord.SelectedWord");
			//UTexture2D* SelectionTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *CheckImagePath));

			CheckImage->SetBrushFromTexture(SelectionTexture);

			UCanvasPanelSlot* checkImageSlot = (UCanvasPanelSlot*)CheckImage->Slot;
			if (checkImageSlot) {

				checkImageSlot->SetSize(FVector2D(spacing, spacing));
				checkImageSlot->SetPosition(FVector2D(imageSlot->GetPosition().X + (spacing * (i + 1)), imageSlot->GetPosition().Y));
				CheckCol++;

			}
		}
	}
	else if (selectedRotation == R_DOWN_DIAGONAL) {
		for (int i = 0; i < word.Len() - 1; i++) {
			UImage* CheckImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), (FName(*FString::Printf(TEXT("Check %d, %d"), CheckRow, CheckCol))));
			Canvas->AddChild(CheckImage);
			CheckImageArray.Add(CheckImage);

			//FString CheckImagePath = FString("/Game/_Main/Puzzles/WordSearch/Texture/SelectedWord.SelectedWord");
			//UTexture2D* SelectionTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *CheckImagePath));

			CheckImage->SetBrushFromTexture(SelectionTexture);

			UCanvasPanelSlot* checkImageSlot = (UCanvasPanelSlot*)CheckImage->Slot;
			if (checkImageSlot) {

				checkImageSlot->SetSize(FVector2D(spacing, spacing));
				checkImageSlot->SetPosition(FVector2D(imageSlot->GetPosition().X + (spacing * (i + 1)), imageSlot->GetPosition().Y + (spacing * (i + 1))));
				CheckRow++;
				CheckCol++;

			}
		}
	}
	else if (selectedRotation == R_DOWN) {
		for (int i = 0; i < word.Len() - 1; i++) {
			UImage* CheckImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), (FName(*FString::Printf(TEXT("Check %d, %d"), CheckRow, CheckCol))));
			Canvas->AddChild(CheckImage);
			CheckImageArray.Add(CheckImage);

			//FString CheckImagePath = FString("/Game/_Main/Puzzles/WordSearch/Texture/SelectedWord.SelectedWord");
			//UTexture2D* SelectionTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *CheckImagePath));

			CheckImage->SetBrushFromTexture(SelectionTexture);

			UCanvasPanelSlot* checkImageSlot = (UCanvasPanelSlot*)CheckImage->Slot;
			if (checkImageSlot) {

				checkImageSlot->SetSize(FVector2D(spacing, spacing));
				checkImageSlot->SetPosition(FVector2D(imageSlot->GetPosition().X, imageSlot->GetPosition().Y + (spacing * (i + 1))));
				CheckRow++;

			}
		}
	}

}

TSharedRef<SWidget> UWordSearchWidget::RebuildWidget()
{
	auto Result = Super::RebuildWidget();

	return Result;
}
// Don't steal out game please :( we worked hard on it!


#include "Puzzles/Hangman/HangmanInputWidget.h"

void UHangmanInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupWidget();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Widget Displayed To Viewport")));
}

void UHangmanInputWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!InputText->HasKeyboardFocus()) {
		//InputText->SetKeyboardFocus();
	}
} 

void UHangmanInputWidget::SetupWidget()
{
	if (WidgetTree) {
		//Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Canvas"));
		WidgetTree->RootWidget = Canvas;
		Canvas->SetVisibility(ESlateVisibility::Visible);

		if (Canvas) {
			//InputText = WidgetTree->ConstructWidget<UEditableText>(UEditableText::StaticClass(), FName("Editable Text"));
			Canvas->AddChild(InputText);
			InputText->SetText(FText::FromString(""));

			UCanvasPanelSlot* textSlot = (UCanvasPanelSlot*)InputText->Slot;
			if (textSlot) {
				FSlateFontInfo currentFont = InputText->GetFont();
				currentFont.Size = 18;
			}
		}
	}

	InputText->OnTextCommitted.AddDynamic(this, &UHangmanInputWidget::TextCommit);
}

void UHangmanInputWidget::TextCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
	CommitedText = Text.ToString();
	InputText->SetText(FText::FromString(""));

	// Runs when player makes guess
	if (CommitMethod == ETextCommit::OnEnter) {
		// Make sure guess text field isn't empty
		if (CommitedText.Len() >= 1) {

		}
	// On attempt to leave puzzle (Escape key is pressed)
	} else if (CommitMethod == ETextCommit::OnCleared) {
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		OnTextCleared.Broadcast();
		//UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}
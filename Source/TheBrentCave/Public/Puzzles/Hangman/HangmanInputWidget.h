// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableText.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Font.h"
#include "Fonts/SlateFontInfo.h"
#include "HangmanInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API UHangmanInputWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextCleared);

	UPROPERTY()
		FOnTextCleared OnTextCleared;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UEditableText* InputText;

	UPROPERTY()
		FString CommitedText;

	UFUNCTION()
		void TextCommit(const FText& Text, ETextCommit::Type CommitMethod);

protected:

	void SetupWidget();
;
	void OnExitPuzzle();

private:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};


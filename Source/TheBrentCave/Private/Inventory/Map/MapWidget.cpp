// Don't steal out game please :( we worked hard on it!


#include "Inventory/Map/MapWidget.h"

void UMapWidget::SetupWidget(int _Dimension, float _WidgetResolution, TArray<FWall> WallsToRemove)
{
	if (WidgetTree) {
		Dimension = _Dimension;

		float TopAndBottomBuffer = _WidgetResolution / _Dimension / 4;
		WidgetResolution = _WidgetResolution;

		CellSize = (_WidgetResolution - TopAndBottomBuffer) / Dimension;

		WidgetTree->RootWidget = Canvas;

		// Settings the size of SizeCanvas and making it clip overflow
		UCanvasPanelSlot* SizeCanvasSlot = (UCanvasPanelSlot*)SizeCanvas->Slot;
		SizeCanvasSlot->SetSize(FVector2D(_WidgetResolution));

		// Scaling the walls canvas to zoom in
		if (VisibleDiameter == 0) {
			VisibleDiameter = Dimension;
		}
		
		RenderScale = FVector2D(Dimension / (float)VisibleDiameter);
		WallsCanvas->SetRenderScale(FVector2D(RenderScale));

		GenerateWalls();
		RemoveWalls(WallsToRemove);

		RebuildWidget();
	}
}


void UMapWidget::SummonWall(FWall WallLocation, int Rotation)
{
	UWallWidget* NewWallWidget = WidgetTree->ConstructWidget<UWallWidget>(WallClass, *FString::Printf(TEXT("%s"), *WallLocation.ToString()));
	WallsCanvas->AddChild(NewWallWidget);
	NewWallWidget->SetRotationAndScale(Rotation, CellSize);

	if (Rotation == UWallWidget::HORIZONTAL)
	{
		((UCanvasPanelSlot*)NewWallWidget->Slot)->SetPosition(FVector2D(WallLocation.maxByRow()[1] * CellSize + (CellSize / 16), WallLocation.maxByRow()[0] * CellSize + (CellSize / 8)));
	}
	else if (Rotation == UWallWidget::VERTICAL)
	{
		((UCanvasPanelSlot*)NewWallWidget->Slot)->SetPosition(FVector2D(WallLocation.maxByCol()[1] * CellSize + (CellSize / 8), WallLocation.maxByCol()[0] * CellSize + (CellSize / 16)));
	}
}


void UMapWidget::GenerateWalls()
{
	for (int row = 0; row < Dimension; row++)
	{
		for (int col = 0; col < Dimension; col++)
		{
			// HorizWall

			SummonWall(FWall(FCell(row - 1, col), FCell(row, col)), UWallWidget::HORIZONTAL);

			// VertWall

			SummonWall(FWall(FCell(row, col - 1), FCell(row, col)), UWallWidget::VERTICAL);
		}
	}

	// Last Horiz Walls
	for (int col = 0; col < Dimension; col++)
	{
		SummonWall(FWall(FCell(Dimension - 1, col), FCell(Dimension, col)), UWallWidget::HORIZONTAL);
	}

	// Last Vert Walls
	for (int row = 0; row < Dimension; row++)
	{
		SummonWall(FWall(FCell(row, Dimension - 1), FCell(row, Dimension)), UWallWidget::VERTICAL);
	}
}


void UMapWidget::RemoveWalls(TArray<FWall> WallsToRemove)
{
	for (FWall wall : WallsToRemove) {
		UWidget* WallToRemove = WidgetTree->FindWidget(*wall.ToString());
		if (WallToRemove) {
			WidgetTree->RemoveWidget(WallToRemove);
		}
	}
}


// Calculates the maximum translation a map can have to stay in bounds and returns a 
// boolean value representing whether the translation parameters are in bounds.
bool UMapWidget::IsTranslationInBounds(float _TranslateX, float _TranslateY, float& MaxTranslation)
{
	MaxTranslation = WidgetResolution * (RenderScale.X - 1) / 2;
	if (FMath::Abs(_TranslateX) >= MaxTranslation || FMath::Abs(_TranslateY) >= MaxTranslation) {
		return false;
	}

	return true;
}


void UMapWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	// Setting the rotation of the PlayerIcon icon
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	AActor* PlayerActor = PlayerController->GetViewTarget();
	float PlayerZRotation = PlayerActor->GetActorRotation().Yaw + 90;

	PlayerIcon->SetRenderTransformAngle(PlayerZRotation);

	AActor* myActor = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	AGrid* myGrid = Cast<AGrid>(myActor);
	// Getting players coordinates
	float PlayerX = PlayerActor->GetActorLocation().X;
	float PlayerY = PlayerActor->GetActorLocation().Y;

	// Getting grid's spawn coordinates
	float GridSpawnX = myGrid->GetActorLocation().X - (1000 * myGrid->GetActorScale3D().X / 2);
	float GridSpawnY = myGrid->GetActorLocation().Y - (1000 * myGrid->GetActorScale3D().Y / 2);

	// Calculating the amount of cells the player is offset from the first cell
	float OffsetCellsX = (PlayerX - GridSpawnX) / (1000 * myGrid->GetActorScale3D().X);
	float OffsetCellsY = (PlayerY - GridSpawnY) / (1000 * myGrid->GetActorScale3D().Y);


	// Calculating translate for widget
	float CenterCell = Dimension / 2.0;
	float TranslateX = ((OffsetCellsX - CenterCell) / CenterCell) * (WidgetResolution / 2) * RenderScale.X * -1;
	float TranslateY = ((OffsetCellsY - CenterCell) / CenterCell) * (WidgetResolution / 2) * RenderScale.Y * -1;

	float MaxTranslation;

	if (IsTranslationInBounds(TranslateX, TranslateY, MaxTranslation)) {
		// Setting translate parameter to WallsCanvas and PlayerIcon
		WallsCanvas->SetRenderTranslation(FVector2D(TranslateX, TranslateY));

		// Putting PlayerIcon in center of the widget
		PlayerIcon->SetRenderTranslation(FVector2D(0));
	} else {
		// Clamping translationX and translationY
		float ClampedTranslateX = FMath::Clamp(TranslateX, -MaxTranslation, MaxTranslation);
		float ClampedTranslateY = FMath::Clamp(TranslateY, -MaxTranslation, MaxTranslation);

		WallsCanvas->SetRenderTranslation(FVector2D(ClampedTranslateX, ClampedTranslateY));

		// Getting the offset for PlayerIcon based on translation and clampedtranslation

		float PlayerIconOffsetX = ClampedTranslateX - TranslateX;
		float PlayerIconOffsetY = ClampedTranslateY - TranslateY;
		PlayerIcon->SetRenderTranslation(FVector2D(PlayerIconOffsetX, PlayerIconOffsetY));
	}
}

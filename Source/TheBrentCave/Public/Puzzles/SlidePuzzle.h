// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "TBCCharacter.h"
#include "GameFramework/Actor.h"
#include "SlidePuzzle.generated.h"

UCLASS()
class THEBRENTCAVE_API ASlidePuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlidePuzzle();

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> CompleteImageWidgetClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USceneComponent* Pivot;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PuzzleBase;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> PuzzlePiece;

	TArray<TArray<AActor*>> PuzzlePieces;
	TArray<int> selectedPiece;

	UPROPERTY()
		UMaterialInstanceDynamic* PuzzleMat;

	UPROPERTY(EditAnywhere)
		TArray<UTexture2D*> PuzzleImageArray;

	UPROPERTY()
		UTexture* PuzzleImage;

	UPROPERTY(EditAnywhere)
		USpotLightComponent* Spotlight;

	UPROPERTY(EditAnywhere)
		UCameraComponent* PuzzleCamera;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere)
		USoundCue* SlideSound;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> DestroySlideActor;

	void GeneratePieces();

	UFUNCTION()
		void SetPieceTexture(int row, int col);

	void ShufflePieces();
	void GetValidMovements(int row, int col);

	float PieceSpacing;

	FVector startPieceLocation;

	TArray<int> randomMissing;

	bool inOverlap;
	bool inPuzzle;

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnterPuzzle();
	void RemovePiecesGlow();
	void ExitPuzzle();

	void SetupInput();
	void Interact();
	void GetInput();

	void DisplaySelectedPiece();

	void MovePiece(bool shuffle = false);

	bool CheckForWin();
	void WinPuzzle();

	FTimerHandle breakPuzzle;

	UMaterialInterface* tempMat;
	UMaterialInstanceDynamic* PieceMat;

	enum Directions {UP, DOWN, LEFT, RIGHT};

	UPROPERTY()
		UUserWidget* CompleteImageWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int PuzzleSize;
};

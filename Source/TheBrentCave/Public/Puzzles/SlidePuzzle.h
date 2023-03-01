// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlidePuzzle.generated.h"

UCLASS()
class THEBRENTCAVE_API ASlidePuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlidePuzzle();

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

	UPROPERTY()
		UMaterialInterface* PuzzleMat;

	UPROPERTY(EditAnywhere)
		TArray<UTexture2D*> PuzzleImageArray;

	UPROPERTY()
		UTexture* PuzzleImage;

	void GeneratePieces();

	UFUNCTION()
		void SetPieceTexture(int row, int col);

	float PieceSpacing;

	FVector startPieceLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int PuzzleSize;
};

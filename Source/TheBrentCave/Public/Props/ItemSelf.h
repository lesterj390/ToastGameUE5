// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBCCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Math/UnrealMathUtility.h"
#include "ItemSelf.generated.h"

UCLASS()
class THEBRENTCAVE_API AItemSelf : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSelf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Variables

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ShelfMesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* ItemSpawn;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractComponent;

	UPROPERTY(EditAnywhere)
		UBoxComponent* Hitbox;

	UPROPERTY()
		ATBCCharacter* HitBoxPlayer;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<TSubclassOf<AInventoryItem>> CommonItems;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<TSubclassOf<AInventoryItem>> UncommonItems;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<TSubclassOf<AInventoryItem>> RareItems;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		float UncommonSpawnRate;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		float RareSpawnRate;

	UPROPERTY(VisibleAnywhere)
		AInventoryItem* ChosenItem;

	bool canPickup;

	// Functions

	void SetupShelf();
	void GenerateItem();
	void PickupItem();

	UFUNCTION()
		void OnOverlapStart(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

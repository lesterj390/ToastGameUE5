// Don't steal out game please :( we worked hard on it!

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/DecalComponent.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class THEBRENTCAVE_API AInventoryItem : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimMontage* EquipAnimation;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimMontage* UseAnimation;

	UPROPERTY(EditAnywhere)
	int ItemQuantity;

	UPROPERTY(EditAnywhere)
	int MaxQuantity;

	UPROPERTY(EditAnywhere, Category = "Shelf")
	FVector ShelfLocation;

	UPROPERTY(EditAnywhere, Category = "Shelf")
	FRotator ShelfRotation;

	UPROPERTY(EditAnywhere, Category = "Shelf")
	FVector ShelfScale;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

protected:


public:
	AInventoryItem();

	virtual void Equip();

	virtual void Unequip();

	virtual void Use();

protected:
	virtual void BeginPlay() override;
};

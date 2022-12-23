// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes.generated.h"

/**
 * 
 */


USTRUCT()
struct FCell
{
	GENERATED_BODY()

protected:
	int row;
	int column;
	int error = -99;

public:
	//Constructors
	FCell();


	FCell(int rowParam, int columnParam);

	//Operators
	int& FCell::operator[](int index);
	

	bool operator==(const FCell otherCell) const;

	bool operator!=(const FCell otherCell) const;

	//FORCEINLINE bool operator!=(const FCell otherCell)
	//{
	//	return !operator==(otherCell);
	//}

public:
	FString ToString();
};


USTRUCT()
struct FWall
{
	GENERATED_BODY()

protected:
	FCell cellA;
	FCell cellB;
	FCell error = FCell();

public:
	//Constructors
	FWall();


	FWall(FCell cellAP, FCell cellBP);



	//Operators
	FCell& operator[](int index);


	bool operator==(const FWall otherWall) const;


	bool operator!=(const FWall otherWall) const;


public:
	FCell maxByRow();


	FCell minByRow();


	FCell maxByCol();


	FCell minByCol();


	FString ToString();
};

UCLASS()
class THEBRENTCAVE_API UDataTypes : public UObject
{
	GENERATED_BODY()
	
};

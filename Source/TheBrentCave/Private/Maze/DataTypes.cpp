// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/DataTypes.h"


///////////////////////////////////////////FCell////////////////////////////////////////////////////////////////////////////////////

//Constructors
FCell::FCell() 
{
	row = 0;
	column = 0;
}

FCell::FCell(int rowP, int columnP)
{
	row = rowP;
	column = columnP;
}

//Operators
int& FCell::operator[](int index)
{
	if (index == 0) {
		return row;
	}
	else if (index == 1) {
		return column;
	}
	else {
		//Using fatal verbosity will crash unreal engine
		UE_LOG(LogTemp, Warning, TEXT("Index out of bounds for FCell"));
		return error;
	}
}

bool FCell::operator==(const FCell otherCell) const
{
	if (row == otherCell.row && column == otherCell.column) {
		return true;
	}
	else {
		return false;
	}
}

bool FCell::operator!=(const FCell otherCell) const
{
	return !operator==(otherCell);
}

//Functions
FString FCell::ToString()
{
	return FString::Printf(TEXT("(%d, %d)"), row, column);
}



///////////////////////////////////////////FWall////////////////////////////////////////////////////////////////////////////////////

//Constructors
FWall::FWall()
{
	cellA = FCell();
	cellB = FCell();
}


FWall::FWall(FCell cellAP, FCell cellBP)
{
	cellA = cellAP;
	cellB = cellBP;

}


//Operators
FCell& FWall::operator[](int index)
{
	if (index == 0) {
		return cellA;
	}
	else if (index == 1) {
		return cellB;
	}
	else {
		//Using fatal verbosity will crash unreal engine
		UE_LOG(LogTemp, Warning, TEXT("Index out of bounds for FWall"));
		return error;
	}
}


bool FWall::operator==(const FWall otherWall) const
{
	if ((cellA == otherWall.cellA && cellB == otherWall.cellB) || (cellA == otherWall.cellB && cellB == otherWall.cellA)) {
		return true;
	}
	else {
		return false;
	}
}


bool FWall::operator!=(const FWall otherWall) const
{
	return !operator==(otherWall);
}


//Functions
FCell FWall::maxByRow() 
{
	if (cellB[0] > cellA[0]) {
		return cellB;
	}
	else {
		return cellA;
	}
}


FCell FWall::minByRow() 
{
	if (cellB[0] < cellA[0]) {
		return cellB;
	}
	else {
		return cellA;
	}
}


FCell FWall::maxByCol() 
{
	if (cellB[1] > cellA[1]) {
		return cellB;
	}
	else {
		return cellA;
	}
}


FCell FWall::minByCol()
{
	if (cellB[1] < cellA[1]) {
		return cellB;
	}
	else {
		return cellA;
	}
}


FString FWall::ToString()
{
	//If same row
	if (cellA[0] == cellB[0]) {
		return FString::Printf(TEXT("%s|%s"), *(minByCol().ToString()), *(maxByCol().ToString()));
	}
	//If same column
	else if (cellA[1] == cellB[1]) {
		return FString::Printf(TEXT("%s|%s"), *(minByRow().ToString()), *(maxByRow().ToString()));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Error: the two cells in wall object are not neighboring."));
		return FString::Printf(TEXT("ERROR"));
	}
}
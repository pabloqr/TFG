// Fill out your copyright notice in the Description page of Project Settings.


#include "LibraryTileMap.h"

#include "ActorTileMap.h"

bool ULibraryTileMap::CheckValidPosition(const FIntPoint& Pos, const FIntPoint& Limit)
{
	return 0 <= Pos.X && Pos.X < Limit.X && 0 <= Pos.Y && Pos.Y < Limit.Y;
}

bool ULibraryTileMap::IsTileTypeAccesible(const ETileType& TileType)
{
	return TileType != ETileType::Ice && TileType != ETileType::Mountains && TileType != ETileType::Water && TileType != ETileType::Max;
}

int32 ULibraryTileMap::IsTileInPath(const TArray<FMovement>& Path, const FIntPoint& Pos)
{
	for (int32 i = 0; i < Path.Num(); ++i)
	{
		if (Path[i].Pos2D == Pos) return i;
	}

	return -1;
}

void ULibraryTileMap::UpdatePathTurns(TArray<FMovement>& Path, const int32 BaseMovementPoints, int32 MovementPoints)
{
	// Se recorren todas las casillas del camino
	for (int32 i = 0; i < Path.Num(); ++i) GetPathTurnAtIndex(Path, i, BaseMovementPoints, MovementPoints);
}

void ULibraryTileMap::GetPathTurnAtIndex(TArray<FMovement>& Path, const int32 Index, const int32 BaseMovementPoints, int32& MovementPoints)
{
	// Se comprueba que el camino tenga elementos y que el indice dado sea correcto
	if (Path.Num() > 0 && 0 <= Index && Index < Path.Num())
	{
		// Se establece el contador de turnos a 0
		int32 Turn = Index > 0 ? Path[Index-1].TotalTurns : 1;

		// Si no hay puntos de movimiento suficientes se aumenta en uno el turno y se restablece el numero de
		// puntos de movimiento
		if (Path[Index].MovementCost > MovementPoints)
		{
			++Turn;
			MovementPoints = BaseMovementPoints;
		}

		// Se actualiza el turno y el numero de puntos de movimiento
		Path[Index].TotalTurns = Turn;
		MovementPoints -= Path[Index].MovementCost;
	}
}

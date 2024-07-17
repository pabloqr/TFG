// Fill out your copyright notice in the Description page of Project Settings.


#include "LibraryTileMap.h"

#include "ActorTileMap.h"

FIntVector ULibraryTileMap::OffsetCoordsToCubeCoords(const FIntPoint& OffsetCoords)
{
	const int32 X = OffsetCoords.Y;
	const int32 Y = OffsetCoords.X - (OffsetCoords.Y - FMath::Abs(OffsetCoords.Y));
	return FIntVector(X, Y, -X - Y);
}

FIntPoint ULibraryTileMap::CubeCoordsToOffsetCoords(const FIntVector& CubeCoords)
{
	return FIntPoint(CubeCoords.Y + (CubeCoords.X - FMath::Abs(CubeCoords.X) / 2), CubeCoords.X);
}

//--------------------------------------------------------------------------------------------------------------------//

bool ULibraryTileMap::CheckValidPosition(const FIntPoint& Pos, const FIntPoint& Limit)
{
	return 0 <= Pos.X && Pos.X < Limit.X && 0 <= Pos.Y && Pos.Y < Limit.Y;
}

//--------------------------------------------------------------------------------------------------------------------//

TArray<FIntPoint> ULibraryTileMap::GetNeighbors(const FIntPoint& Pos, const FIntPoint& MapSize)
{
	// Se preasigna el desplazamiento en funcion de si la columna es par o impar
	TArray<FIntPoint> AllNeighbors = Pos.Y % 2 == 0
		? TArray<FIntPoint>{
			 FIntPoint(-1, -1), FIntPoint(-1, 0), FIntPoint(-1, 1), FIntPoint(0, 1),
			 FIntPoint(1, 0), FIntPoint(0, -1)
		}
		: TArray<FIntPoint>{
			 FIntPoint(0, -1), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(1, 1),
			 FIntPoint(1, 0), FIntPoint(1, -1)
		};

	// Se actualizan las posiciones de los vecinos y se comprueba si son correctas
	TArray<FIntPoint> Neighbors;
	for (int32 i = 0; i < AllNeighbors.Num(); ++i)
	{
		AllNeighbors[i] += Pos;
		if (CheckValidPosition(AllNeighbors[i], MapSize)) Neighbors.Add(AllNeighbors[i]);
	}

	return Neighbors;
}

int32 ULibraryTileMap::GetDistanceToElement(const FIntPoint& PosIni, const FIntPoint& PosEnd)
{
	const FIntVector Distance = OffsetCoordsToCubeCoords(PosEnd) - OffsetCoordsToCubeCoords(PosIni);
	return FMath::Max3(Distance.X, Distance.Y, Distance.Z);
}

//--------------------------------------------------------------------------------------------------------------------//

int32 ULibraryTileMap::GetTileCostFromType(const ETileType TileType)
{
	int32 MovementCost;
	switch (TileType)
	{
	case ETileType::Plains: MovementCost = 1;
		break;
	case ETileType::Hills: MovementCost = 2;
		break;
	case ETileType::Forest: MovementCost = 2;
		break;
	case ETileType::SnowPlains: MovementCost = 1;
		break;
	case ETileType::SnowHills: MovementCost = 2;
		break;
	case ETileType::Ice:
	case ETileType::Mountains:
	case ETileType::Water: MovementCost = -1;
		break;
	default: MovementCost = -1;
		break;
	}

	return MovementCost;
}

bool ULibraryTileMap::IsTileTypeAccesible(const ETileType& TileType)
{
	return GetTileCostFromType(TileType) != -1;
}

//--------------------------------------------------------------------------------------------------------------------//

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

void ULibraryTileMap::GetPathTurnAtIndex(TArray<FMovement>& Path, const int32 Index, const int32 BaseMovementPoints,
                                         int32& MovementPoints)
{
	// Se comprueba que el camino tenga elementos y que el indice dado sea correcto
	if (Path.Num() > 0 && 0 <= Index && Index < Path.Num())
	{
		// Se establece el contador de turnos a 0
		int32 Turn = Index > 0 ? Path[Index - 1].TotalTurns : 1;

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

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

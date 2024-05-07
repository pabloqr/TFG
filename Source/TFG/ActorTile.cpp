// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorTile.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>

AActorTile::AActorTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);
	
	TileInfo = FTileInfo();
}

//--------------------------------------------------------------------------------------------------------------------//

int32 AActorTile::TileTypeToInt(const ETileType TileType)
{
	int32 TileTypeVal;
	switch (TileType)
	{
		case ETileType::Plains: TileTypeVal = 1; break;
		case ETileType::Hills: TileTypeVal = 2; break;
		case ETileType::Forest: TileTypeVal = 3; break;
		case ETileType::SnowPlains: TileTypeVal = 4; break;
		case ETileType::SnowHills: TileTypeVal = 5; break;
		case ETileType::Ice: TileTypeVal = 6; break;
		case ETileType::Mountains: TileTypeVal = 7; break;
		case ETileType::Water: TileTypeVal = 8; break;
		default: TileTypeVal = 0; break;
	}

	return TileTypeVal;
}

ETileType AActorTile::IntToTileType(const int32 TileTypeVal)
{
	ETileType TileType;
	switch (TileTypeVal)
	{
		case 1: TileType = ETileType::Plains; break;
		case 2: TileType = ETileType::Hills; break;
		case 3: TileType = ETileType::Forest; break;
		case 4: TileType = ETileType::SnowPlains; break;
		case 5: TileType = ETileType::SnowHills; break;
		case 6: TileType = ETileType::Ice; break;
		case 7: TileType = ETileType::Mountains; break;
		case 8: TileType = ETileType::Water; break;
		default: TileType = ETileType::None; break;
	}

	return TileType;
}

FTileInfo AActorTile::GetInfo() const
{
	return TileInfo;
}

FIntPoint AActorTile::GetMapPosition() const
{
	return TileInfo.Pos2D;
}

FVector2D AActorTile::GetScenePosition() const
{
	return TileInfo.MapPos2D;
}

ETileType AActorTile::GetType() const
{
	return TileInfo.TileType;
}

TArray<ETileState> AActorTile::GetState() const
{
	return TileInfo.TileStates;
}

void AActorTile::SetInfo(const FTileInfo& Info)
{
	SetPosition(Info.Pos2D, Info.MapPos2D);
	SetType(Info.TileType);
	SetState(Info.TileStates);
}

void AActorTile::SetPosition(const FIntPoint& Pos2D)
{
	TileInfo.Pos2D = FIntPoint(Pos2D);
}

void AActorTile::SetPosition(const FVector2D& MapPos2D)
{
	TileInfo.MapPos2D = FVector2D(MapPos2D);
}

void AActorTile::SetPosition(const FIntPoint& Pos2D, const FVector2D& MapPos2D)
{
	SetPosition(Pos2D);
	SetPosition(MapPos2D);
}

void AActorTile::SetType(const ETileType TileType)
{
	TileInfo.TileType = TileType;
}

void AActorTile::SetState(const TArray<ETileState>& TileStates)
{
	TileInfo.TileStates.Empty();
	AddState(TileStates);
}

void AActorTile::SetState(const ETileState TileState)
{
	TileInfo.TileStates.Empty();
	AddState(TileState);
}

void AActorTile::AddState(const TArray<ETileState>& TileStates)
{
	for (const ETileState State : TileStates) AddState(State);
}

void AActorTile::AddState(const ETileState TileState)
{
	TileInfo.TileStates.AddUnique(TileState);
}

void AActorTile::RemoveState(const TArray<ETileState>& TileStates)
{
	for (const ETileState State : TileStates) RemoveState(State);
}

void AActorTile::RemoveState(const ETileState TileState)
{
	if (TileInfo.TileStates.Contains(TileState)) TileInfo.TileStates.Remove(TileState);
}

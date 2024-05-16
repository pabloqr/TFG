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

	MovementCost = 1;
	
	TileStates.Add(ETileState::None);
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
	return FTileInfo(Pos2D, MapPos2D, TileType);
}

const ETileType& AActorTile::GetType() const
{
	return TileType;
}

int32 AActorTile::GetMovementCost() const
{
	return MovementCost;
}

const TArray<ETileState>& AActorTile::GetState() const
{
	return TileStates;
}

void AActorTile::SetPos(const FTileInfo& Pos)
{
	AActorPlaceableElement::SetPos(Pos.Pos2D, Pos.MapPos2D);
}

void AActorTile::SetType(const ETileType Type)
{
	TileType = Type;
}

void AActorTile::SetMovementCost(const int32 Cost)
{
	MovementCost = Cost;
}

void AActorTile::SetState(const TArray<ETileState>& States)
{
	TileStates.Empty();
	AddState(States);
}

void AActorTile::SetState(const ETileState State)
{
	TileStates.Empty();
	AddState(State);
}

void AActorTile::AddState(const TArray<ETileState>& States)
{
	for (const ETileState State : States) AddState(State);
}

void AActorTile::AddState(const ETileState State)
{
	TileStates.AddUnique(State);
}

void AActorTile::RemoveState(const TArray<ETileState>& States)
{
	for (const ETileState State : States) RemoveState(State);
}

void AActorTile::RemoveState(const ETileState State)
{
	if (TileStates.Contains(State)) TileStates.Remove(State);
}

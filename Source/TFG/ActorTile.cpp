// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorTile.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>

#include "ActorUnit.h"
#include "ActorSettlement.h"
#include "LibraryTileMap.h"

AActorTile::AActorTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);

	Info = FTileInfo();
}

//--------------------------------------------------------------------------------------------------------------------//

int32 AActorTile::GetMovementCost() const
{
	return ULibraryTileMap::GetTileCostFromType(Info.Type);
}

const AActorDamageableElement* AActorTile::GetElement() const
{
	if (Info.Elements.Unit) return Info.Elements.Unit;
	if (Info.Elements.Settlement) return Info.Elements.Settlement;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorTile::SetPos(const FIntPoint& Pos, const FVector2D& MapPos)
{
	Info.Pos2D = Pos;
	Info.MapPos2D = MapPos;
}

void AActorTile::SetType(const ETileType TileType)
{
	Info.Type = TileType;
}

void AActorTile::SetState(const TArray<ETileState>& TileStates)
{
	Info.States.Empty();
	AddState(TileStates);
}

void AActorTile::SetState(const ETileState State)
{
	Info.States.Empty();
	AddState(State);
}

void AActorTile::AddState(const TArray<ETileState>& TileStates)
{
	for (const ETileState State : TileStates) AddState(State);
}

void AActorTile::AddState(const ETileState State)
{
	if (State != ETileState::None) Info.States.Remove(ETileState::None);
	else Info.States.Empty();

	Info.States.AddUnique(State);
}

void AActorTile::RemoveState(const TArray<ETileState>& TileStates)
{
	for (const ETileState State : TileStates) RemoveState(State);
}

void AActorTile::RemoveState(const ETileState State)
{
	Info.States.Remove(State);
	if (Info.States.Num() == 0) Info.States.Add(ETileState::None);
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorTile::IsAccesible() const
{
	return ULibraryTileMap::GetTileCostFromType(Info.Type) != -1;
}

bool AActorTile::HasElement() const
{
	return HasUnit() || HasSettlement();
}

bool AActorTile::HasUnit() const
{
	return Info.Elements.Unit != nullptr;
}

bool AActorTile::HasSettlement() const
{
	return Info.Elements.Settlement != nullptr;
}

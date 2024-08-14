// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorTile.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>

#include "ActorResource.h"
#include "ActorUnit.h"
#include "ActorSettlement.h"
#include "LibraryTileMap.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"

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

const AActorResource* AActorTile::GetResource() const
{
	return Info.Elements.Resource;
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

void AActorTile::SetResource(AActorResource* Resource)
{
	// Si la casilla tiene un recurso, se elimina
	if (Info.Elements.Resource) Info.Elements.Resource->Destroy();

	Info.Elements.Resource = Resource;
}

void AActorTile::SetUnit(AActorUnit* Unit)
{
	Info.Elements.Unit = Unit;
}

void AActorTile::SetSettlement(AActorSettlement* Settlement)
{
	Info.Elements.Settlement = Settlement;
}

void AActorTile::SetState(const TSet<ETileState>& TileStates)
{
	// Se eliminan los estados actuales y se anaden los nuevos
	Info.States.Empty();
	AddState(TileStates);
}

void AActorTile::SetState(const ETileState State)
{
	// Se eliminan los estados actuales y se anade el nuevo
	Info.States.Empty();
	AddState(State);
}

void AActorTile::AddState(const TSet<ETileState>& TileStates)
{
	// Se procesan todos los estados dados
	for (const ETileState State : TileStates) AddState(State);
}

void AActorTile::AddState(const ETileState State)
{
	// Si el estado no es 'None', se elimina en caso de estar contenido, en caso contrario, se vacia la lista
	if (State != ETileState::None) Info.States.Remove(ETileState::None);
	else Info.States.Empty();

	// Se anade el estado
	Info.States.Add(State);
}

void AActorTile::RemoveState(const TSet<ETileState>& TileStates)
{
	// Se procesan todos los estados dados
	for (const ETileState State : TileStates) RemoveState(State);
}

void AActorTile::RemoveState(const ETileState State)
{
	// Se elimina el estado
	Info.States.Remove(State);
	// Si no hay mas estados, se anade el estado 'None'
	if (Info.States.Num() == 0) Info.States.Add(ETileState::None);
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorTile::IsMine() const
{
	// Se verifica si la unidad pertenece a la faccion en juego
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		return Info.Owner == State->GetCurrentIndex();
	}

	return false;
}

bool AActorTile::IsAccesible() const
{
	return ULibraryTileMap::GetTileCostFromType(Info.Type) != -1;
}

bool AActorTile::HasElement() const
{
	return HasUnit() || HasSettlement();
}

bool AActorTile::HasResource() const
{
	return Info.Elements.Resource != nullptr;
}

bool AActorTile::HasUnit() const
{
	return Info.Elements.Unit != nullptr;
}

bool AActorTile::HasSettlement() const
{
	return Info.Elements.Settlement != nullptr;
}

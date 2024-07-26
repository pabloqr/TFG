// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSettlement.h"

#include "FProductionElement.h"
#include "GInstance.h"
#include "LibraryDataTables.h"
#include "LibraryTileMap.h"
#include "Kismet/GameplayStatics.h"


AActorSettlement::AActorSettlement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Info = FSettlementInfo();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::SetInitialOwnedTiles()
{
	// Se obtiene el tamano del mapa para verificar las posiciones
	FIntPoint MapSize;

	if (const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		MapSize = GameInstance->Size2D;
	}

	for (const FIntPoint Pos : ULibraryTileMap::GetNeighbors(Info.Pos2D, MapSize))
	{
		// Se anade la casilla a la lista
		Info.OwnedTiles.Add(Pos);

		// Se llama al evento para actualizar el propietario de la casilla
		OnTileOwned.Broadcast(Pos);
	}

	// Se realiza el mismo proceso para la casilla en la que se situa el asentamiento
	Info.OwnedTiles.AddUnique(Info.Pos2D);
	OnTileOwned.Broadcast(Info.Pos2D);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::AddToProductionQueue(const UDataTable* DataTable, const TSubclassOf<AActorUnit> UnitClass,
                                            const EUnitType UnitType)
{
	// Se comprueba si existe una entrada en la lista de elementos comenzados para continuar su produccion
	const int32 Index = Info.StartedProduction.IndexOfByPredicate([&](const FProductionElement& Element)
	{
		return UnitType == Element.UnitType;
	});

	if (Index != INDEX_NONE)
	{
		// Si existe una entrada, se anade a la cola
		Info.ProductionQueue.Add(Info.StartedProduction[Index]);

		// Se elimina de la lista la entrada correspondiente
		Info.StartedProduction.RemoveAt(Index);
	}
	else
	{
		// Si no existe una entrada, se obtiene la informacion de la unidad
		const FUnitData UnitData = ULibraryDataTables::GetUnitDataFromType(DataTable, UnitType);

		// Se anade el elemento a la cola de produccion
		Info.ProductionQueue.Add(FProductionElement(UnitClass, UnitData.Type, UnitData.ProductionCost / 10.0));
	}

	// Se actualiza el estado
	Info.State = ESettlementState::Producing;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, Info.State);
}

void AActorSettlement::RemoveFromProduction(const UDataTable* DataTable, const int32 Index)
{
	// Se verifica que el indice sea valido
	if (0 <= Index && Index < Info.ProductionQueue.Num())
	{
		// Se obtiene la informacion de la unidad y se comprueba si ya se ha trabajado en ella algun turno
		const FUnitData UnitData = ULibraryDataTables::GetUnitDataFromType(
			DataTable, Info.ProductionQueue[Index].UnitType);
		if (Info.ProductionQueue[Index].ProductionCost < UnitData.ProductionCost / 10.0)
		{
			// Si ya se ha trabajado en la unidad algun turno, se anade la entrada a la listaZ
			Info.StartedProduction.Add(Info.ProductionQueue[Index]);
		}

		// Se elimina el elemento de la lista
		Info.ProductionQueue.RemoveAt(Index);

		// Se actualiza el estado
		Info.State = Info.ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;

		// Se llama al evento tras actualizar el estado
		OnSettlementStateChanged.Broadcast(this, Info.State);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::TurnStarted()
{
	// Se comprueba si hay elementos en produccion
	if (Info.ProductionQueue.Num() > 0)
	{
		// Se actualiza el coste de produccion
		Info.ProductionQueue[0].ProductionCost -= 1;

		// Se comprueba si se han completado todos los turnos para producir la unidad
		if (Info.ProductionQueue[0].ProductionCost <= 0)
		{
			// Se obtiene la unidad a generar
			const FProductionElement UnitToSpawn = Info.ProductionQueue[0];

			// Se elimina la unidad de la cola de produccion
			Info.ProductionQueue.RemoveAt(0);

			// Se llama al evento para generar la unidad en el mapa
			OnUnitProduced.Broadcast(Info.Pos2D, UnitToSpawn);
		}
	}

	// Se actualiza el estado
	Info.State = Info.ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, Info.State);
}

void AActorSettlement::TurnEnded()
{
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

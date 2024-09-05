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

	// Se inicializa la referencia al mapa a un valor nulo
	TileMap = nullptr;

	// Se inicializa la informacion del asentamiento
	Info = FSettlementInfo();

	// Se establecen los turnos para adquirir una nueva casilla
	Info.TurnsToOwnTile = 5;

	// Se establece el dinero inicial que produce el asentamiento
	Info.MoneyYield = 5.0;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::OwnTile(const FIntPoint& Pos)
{
	// Se verifica que la instancia del mapa sea valida y la casilla no este ya en propiedad de alguna faccion
	if (!TileMap || TileMap->IsTileOwned(Pos)) return;

	// Se anade la casilla a la lista
	Info.OwnedTiles.Add(Pos);

	// Se llama al evento para actualizar el propietario de la casilla
	OnTileOwned.Broadcast(Pos);
}

void AActorSettlement::OwnTileInRange()
{
	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return;

	// Se crean contenedores para almacenar las casillas de los anillos exteriores
	TArray<FIntPoint> RangeTwo = TArray<FIntPoint>();
	TArray<FIntPoint> RangeThree = TArray<FIntPoint>();

	// Se obtienen todas las casillas dentro del rango de expansion y se procesan
	TArray<FIntPoint> InRange = TileMap->GetTilesWithinRange(Info.Pos2D, 3, false, false);
	for (const auto TilePos : InRange)
	{
		// Si la casilla ya tiene propietario, se omite
		if (!TileMap->IsTileOwned(TilePos))
		{
			// Se calcula la distancia
			const int32 Distance = ULibraryTileMap::GetDistanceToElement(Info.Pos2D, TilePos);

			// Se clasifica segun el resultado
			if (Distance == 2) RangeTwo.Add(TilePos);
			else if (Distance == 3) RangeThree.Add(TilePos);
		}
	}

	// Si se han obtenido casillas, se establece la propiedad de una de ellas
	if (RangeTwo.Num() > 0) OwnTile(RangeTwo[0]);
	else if (RangeThree.Num() > 0) OwnTile(RangeThree[0]);

	// Se actualiza el contador de turnos
	Info.TurnsToOwnTile = 5;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::SetInitialOwnedTiles()
{
	// Se obtiene el tamano del mapa para verificar las posiciones
	FIntPoint MapSize = FIntPoint(0);

	if (const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		MapSize = GameInstance->Size2D;
	}

	// Se actualizan las casillas para que sean propiedad de la faccion actual
	for (const FIntPoint Pos : ULibraryTileMap::GetNeighbors(Info.Pos2D, MapSize)) OwnTile(Pos);

	// Se realiza el mismo proceso para la casilla en la que se situa el asentamiento
	OwnTile(Info.Pos2D);
}

void AActorSettlement::DisownTile(const FIntPoint& Pos)
{
	// Se elimina la casilla de la lista
	Info.OwnedTiles.Remove(Pos);

	// Se llama al evento para actualizar el propietario de la casilla
	OnTileDisowned.Broadcast(Pos);
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

		// Se llama al evento para actualizar los recursos de la faccion
		OnUnitProductionStarted.Broadcast(UnitType);
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
			// Si ya se ha trabajado en la unidad algun turno, se anade la entrada a la lista
			Info.StartedProduction.Add(Info.ProductionQueue[Index]);
		}
		else
		{
			// Se llama al evento para actualizar los recursos de la faccion
			OnUnitProductionCancelled.Broadcast(UnitData.Type);
		}

		// Se elimina el elemento de la lista
		Info.ProductionQueue.RemoveAt(Index);

		// Se actualiza el estado
		Info.State = Info.ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;

		// Se llama al evento tras actualizar el estado
		OnSettlementStateChanged.Broadcast(this, Info.State);
	}
}

void AActorSettlement::ResetProduction()
{
	// Si hay un elemento en produccion se cancela
	if (Info.ProductionQueue.Num() > 0)
	{
		// Se llama al evento para actualizar los recursos de la faccion
		OnUnitProductionCancelled.Broadcast(Info.ProductionQueue[0].UnitType);
	}
	
	// Se limpian las colas de produccion
	Info.ProductionQueue.Empty();
	Info.StartedProduction.Empty();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::ApplyDamage(const float Damage, const AActorDamageableElement* Element)
{
	Super::ApplyDamage(Damage, Element);

	// Si no quedan puntos de vida, se llama al evento que gestiona la destruccion de la unidad
	if (DamageableInfo.HealthPoints <= 0.0) OnSettlementConquered.Broadcast(Element, this);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::TurnStarted()
{
	// Se actualizan los puntos de vida
	DamageableInfo.HealthPoints = FMath::Min(DamageableInfo.HealthPoints + 10.0f, DamageableInfo.BaseHealthPoints);

	// Se actualizan los atributos de ataque de acuerdo a la vida restante
	UpdateAttackAndDefenseParameters();
	
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
	// Se actualiza el contador para la expansion y se obtiene una nueva casilla si procede
	if (--Info.TurnsToOwnTile <= 0) OwnTileInRange();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

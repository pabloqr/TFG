// Fill out your copyright notice in the Description page of Project Settings.


#include "CMainAI.h"

#include "ActorSettlement.h"
#include "ActorTileMap.h"
#include "LibraryTileMap.h"
#include "MMain.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACMainAI::ACMainAI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializa la faccion a un valor nulo
	PawnFaction = nullptr;

	// Se inicializa el mapa a un valor nulo
	TileMap = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

const AActorSettlement* ACMainAI::GetClosestSettlementFromPos(const FIntPoint& Pos,
                                                              const TArray<AActorSettlement*>& Settlements)
{
	const AActorSettlement* SettlementPos = nullptr;

	// Se calcula la posicion del asentamiento mas cercano
	int32 MinDistance = 999;
	for (const auto Settlement : Settlements)
	{
		// Se obtiene la distancia y, si es menor, se actualizan las variables de la posicion y distancia
		const int32 Distance = ULibraryTileMap::GetDistanceToElement(Pos, Settlement->GetPos());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			SettlementPos = Settlement;
		}
	}

	return SettlementPos;
}

FIntPoint ACMainAI::GetClosestTilePosition(const FIntPoint& Pos, TArray<FIntPoint>& SettlementOwnedTiles) const
{
	FIntPoint ClosestPos = Pos;

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return ClosestPos;

	// Variable que almacena la casilla mas cercana
	for (int i = 0; i < SettlementOwnedTiles.Num(); ++i)
	{
		// Se obtiene la casilla mas cercana de todas las de la lista
		const FIntPoint ClosestSettlementTile = ULibraryTileMap::GetClosestElementFromPos(
			Pos, TSet<FIntPoint>(SettlementOwnedTiles));

		// Si la casilla no contiene ningun elemento, se actualizan las variables y se finaliza el bucle
		if (!TileMap->TileHasElement(ClosestSettlementTile))
		{
			ClosestPos = ClosestSettlementTile;
			break;
		}

		// Se elimina la casilla de la lista
		SettlementOwnedTiles.RemoveAt(i);

		// Se actualiza el indice
		--i;
	}

	return ClosestPos;
}

//--------------------------------------------------------------------------------------------------------------------//

TSet<FIntPoint> ACMainAI::GetEnemyOrAllyLocationInRange(const FIntPoint& Pos, const int32 Range,
                                                        const bool GetEnemy) const
{
	// Coleccion de casillas con enemigos
	TSet<FIntPoint> ElementsLocation = TSet<FIntPoint>();

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return ElementsLocation;

	// Se obtienen las casillas al alcance de la unidad y se procesan
	TArray<FIntPoint> TilesInRange = TileMap->GetTilesWithinRange(Pos, Range, false);
	for (auto TilePos : TilesInRange)
	{
		// Si la casilla tiene un enemigo, se anade a la coleccion
		if (TileMap->TileHasEnemyOrAlly(TilePos, GetEnemy)) ElementsLocation.Add(TilePos);
	}

	return ElementsLocation;
}

const AActorSettlement* ACMainAI::GetClosestEnemySettlementFromPos(const FIntPoint& Pos) const
{
	const AActorSettlement* Settlement = nullptr;

	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		// Variable que almacena la distancia al asentamiento mas cercano
		int32 MinDistance = 999;

		// Se obtienen las facciones y se procesan las que aun estan en juego
		const TMap<int32, APawnFaction*> Factions = State->GetFactions();
		for (const auto FactionIndex : State->GetFactionsAlive())
		{
			// Si el indice no esta contenido, se omite
			if (Factions.Contains(FactionIndex))
			{
				// Se obtienen los asentamientos y se obtiene el mas cercano
				const AActorSettlement* ClosestSettlement =
					GetClosestSettlementFromPos(Pos, Factions[FactionIndex]->GetSettlements());

				// Se obtiene la distancia y, si es menor que la actual, se actualizan las variables
				const int32 Distance = ULibraryTileMap::GetDistanceToElement(Pos, ClosestSettlement->GetPos());
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					Settlement = ClosestSettlement;
				}
			}
		}
	}

	return Settlement;
}

const AActorSettlement* ACMainAI::GetClosestOwnedSettlementFromPos(const FIntPoint& Pos) const
{
	// Si la referencia no es valida, se devuelve un valor nulo
	if (!PawnFaction) return nullptr;

	// Se obtiene el asentamiento mas cercano
	return GetClosestSettlementFromPos(Pos, PawnFaction->GetSettlements());
}

FIntPoint ACMainAI::GetFarthestPositionFromEnemies(const FIntPoint& Pos, const int32 Range) const
{
	FIntPoint FarthestPos = Pos;

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return FarthestPos;

	// Se obtienen las casillas al alcance de la unidad y se procesan
	TArray<FIntPoint> TilesInRange = TileMap->GetTilesWithinRange(Pos, Range, true);

	// Variable que almacena la mayor distancia 
	int32 MaxDistance = 0;

	// Se calcula la casilla mas alejada de los enemigos teniendo en cuenta el enemigo mas cercano
	for (const auto TilePos : TilesInRange)
	{
		// Si la casilla contiene un enemigo, se omite
		if (EnemiesLocation.Contains(TilePos)) continue;

		// Se obtiene la posicion del enemigo mas cercano
		FIntPoint ClosestEnemy = ULibraryTileMap::GetClosestElementFromPos(TilePos, EnemiesLocation);
		// Variable que almacena la distancia al enemigo mas cercano
		const int32 MinDistance = ULibraryTileMap::GetDistanceToElement(TilePos, ClosestEnemy);

		// Se actualiza la distancia y la posicion
		if (MinDistance > MaxDistance)
		{
			MaxDistance = MinDistance;
			FarthestPos = TilePos;
		}
	}

	return FarthestPos;
}

FIntPoint ACMainAI::GetClosestPositionToAlly(const FIntPoint& Pos) const
{
	FIntPoint ClosestPos = Pos;

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return ClosestPos;

	// Se obtiene la posicion del aliado mas cercano
	const FIntPoint AllyPos = ULibraryTileMap::GetClosestElementFromPos(Pos, AlliesLocation);

	// Se van a crear variables que almacenen una posicion provisional mas cercana que puede contener ya elementos
	int32 MinDistance = 999;
	FIntPoint ClosestProvPos = AllyPos;

	while (ClosestPos != ClosestProvPos)
	{
		// Se calcula la casilla adyacente mas cercana, para ello, se obtienen los vecinos
		TArray<FIntPoint> Neighbors = ULibraryTileMap::GetNeighbors(ClosestProvPos, TileMap->GetSize());
		for (const auto NeighborPos : Neighbors)
		{
			// Se obtiene la distancia a la casilla
			const int32 Distance = ULibraryTileMap::GetDistanceToElement(Pos, NeighborPos);

			// Se comprueba que la distancia sea menor a la ya almacenada
			if (Distance < MinDistance)
			{
				// Se actualiza la distancia
				MinDistance = Distance;

				// Si la casilla no contiene ninguna unidad, se actualiza la posicion final
				if (!TileMap->TileHasElement(NeighborPos))
				{
					ClosestPos = NeighborPos;
				}

				// Si la distancia es 0, quiere decir que se ha llegado a la posicion de la unidad y, por tanto,
				// se devuelve la posicion valida que se tenga actualmente o se mantiene la posicion
				if (MinDistance == 0)
				{
					// TODO: revisar para no devolver la posicion actual en caso de fallo (no es prioritario)
					ClosestProvPos = ClosestPos != -1 ? ClosestPos : Pos;
					break;
				}

				// Se actualiza la posicion provisional
				ClosestProvPos = NeighborPos;
			}
		}
	}

	return ClosestPos;
}

FIntPoint ACMainAI::GetClosestEnemyTilePosition(const FIntPoint& Pos) const
{
	FIntPoint ClosestPos = Pos;

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return ClosestPos;

	// Se obtienen las casillas en posesion del asentamiento enemigo mas cercano
	TArray<FIntPoint> SettlementOwnedTiles = GetClosestEnemySettlementFromPos(Pos)->GetOwnedTiles();

	// Se obtiene la casilla mas cercana sin ocupar y se devuelve
	return GetClosestTilePosition(Pos, SettlementOwnedTiles);
}

FIntPoint ACMainAI::GetClosestAllyTilePosition(const FIntPoint& Pos) const
{
	FIntPoint ClosestPos = Pos;

	// Se verifica que la instancia del mapa sea valida
	if (!TileMap) return ClosestPos;

	// Se obtienen las casillas en posesion del asentamiento propio mas cercano
	TArray<FIntPoint> SettlementOwnedTiles = GetClosestOwnedSettlementFromPos(Pos)->GetOwnedTiles();

	// Se obtiene la casilla mas cercana sin ocupar y se devuelve
	return GetClosestTilePosition(Pos, SettlementOwnedTiles);
}

FIntPoint ACMainAI::CalculateBestPositionForUnit(const AActorUnit* Unit, const EUnitAction UnitAction) const
{
	// Si se debe mover hacia un enemigo, se obtiene el mas cercano
	if (UnitAction == EUnitAction::MoveTowardsEnemy)
	{
		return ULibraryTileMap::GetClosestElementFromPos(Unit->GetPos(), EnemiesLocation);
	}
	// Si debe huir de un enemigo, se obtiene la posicion mas alejada en conjunto de todos los enemigos
	if (UnitAction == EUnitAction::MoveAwayFromEnemy)
	{
		return GetFarthestPositionFromEnemies(Unit->GetPos(), Unit->GetMovementPoints());
	}
	// Si se debe mover hacia un aliado, se obtiene el mas cercano y la posicion mas cercana a este que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsAlly)
	{
		return GetClosestPositionToAlly(Unit->GetPos());
	}
	// Si se debe mover hacia una casilla enemiga, se obtiene la mas cercana que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsEnemyTiles)
	{
		return GetClosestEnemyTilePosition(Unit->GetPos());
	}
	// Si se debe mover hacia una casilla aliada, se obtiene la mas cercana que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsAllyTiles)
	{
		return GetClosestAllyTilePosition(Unit->GetPos());
	}
	// Si debe explorar, se obtiene aleatoriamente una casilla dentro del alcance de movimiento
	if (UnitAction == EUnitAction::MoveAround)
	{
		// Se obtienen las casillas al alcance de la unidad y se procesan para eliminar las ocupadas
		TArray<FIntPoint> TilesInRange = TileMap->GetTilesWithinRange(Unit->GetPos(), Unit->GetMovementPoints(), true);
		for (int32 i = 0; i < TilesInRange.Num(); ++i)
		{
			if (EnemiesLocation.Contains(TilesInRange[i]) || AlliesLocation.Contains(TilesInRange[i]))
			{
				// Si la casilla esta ocupada, se elimina la casilla de la lista
				TilesInRange.RemoveAt(i);

				// Se actualiza el indice
				--i;
			}
		}

		// Se verifica si la lista contiene elementos
		if (TilesInRange.Num() != 0)
		{
			// Se obtiene un indice de forma aleatoria para la casilla a la que se debe mover la unidad
			const int32 TileIndex = FMath::RandRange(0, TilesInRange.Num() - 1);

			return TilesInRange[TileIndex];
		}
	}

	// Si la accion no es correcta, se devuelve la posicion actual
	return Unit->GetPos();
}

//--------------------------------------------------------------------------------------------------------------------//

void ACMainAI::ManageUnits()
{
	// Si la faccion no es valida, no se hace nada
	if (!PawnFaction) return;

	const TArray<AActorUnit*> Units = PawnFaction->GetUnits();
	for (const auto Unit : Units)
	{
		// Se obtienen los atributos de la unidad para poder usarlos durante el proceso
		const FUnitInfo UnitInfo = Unit->GetInfo();
		const FVector2D Health = FVector2D(Unit->GetBaseHealthPoints(), Unit->GetHealthPoints());

		// Se calcula el porcentaje de salud
		const float HealthPercentage = Health.Y / Health.X;

		// Primero: se comprueba si hay enemigos cerca
		EnemiesLocation = GetEnemyOrAllyLocationInRange(UnitInfo.Pos2D, UnitInfo.BaseMovementPoints, true);

		// Tambien se obtienen las posiciones de las unidades aliadas cercanas
		AlliesLocation = GetEnemyOrAllyLocationInRange(UnitInfo.Pos2D, UnitInfo.BaseMovementPoints, false);

		// Segundo:
		//		(1) Si hay enemigos:
		//				(1.1) Se comprueba la salud: si esta baja, se cura o se huye. En caso contrario, se verifica para atacar
		//				(1.2) Se verifica para atacar: se verifican las estadisticas de ataque y se decide si se ataca o no
		//		(2) Si no hay enemigos:
		//				(2.1) Se comprueba la salud para curar
		//				(2.2) O se mueve la unidad
		//				(2.3) O se defiende en la posicion
		EUnitAction UnitAction = EUnitAction::None;
		if (EnemiesLocation.Num() > 0) // (1)
		{
			// En caso de que la unidad tenga un camino asignado a una casilla propia, no se hace nada
			if (UnitInfo.Path.Num() == 0 || !TileMap->IsTileMine(UnitInfo.Path.Last().Pos2D))
			{
				// Segun el porcentaje de salud de la unidad:
				//		* 0.0-0.3: se mueve la unidad a una casilla aliada
				//		* 0.3-0.5: se desplaza la unidad lejos del enemigo
				//		* 0.6-1.0: se ataca a la unidad enemiga
				UnitAction = HealthPercentage < 0.3
					             ? EUnitAction::MoveTowardsAllyTiles
					             : 0.3 <= HealthPercentage && HealthPercentage < 0.5
					             ? EUnitAction::MoveAwayFromEnemy
					             : EUnitAction::MoveTowardsEnemy;
			}
		}
		else // (2)
		{
			// Segun el porcentaje de salud de la unidad:
			//		* 0.0-0.7: se cura la unidad
			//		* 0.7-1.0: se explora el mapa
			UnitAction = HealthPercentage < 0.7 ? EUnitAction::Heal : EUnitAction::MoveAround;
		}

		// Dependiendo de la accion se aplican las modificaciones necesarias
		//		(a) Si no se hace nada, no se aplican modificaciones
		//		(b) Si se debe curar, se aplica la curacion a la unidad
		//		(c) En caso contrario, se calcula el camino a seguir dada la accion calculada
		switch (UnitAction)
		{
		case EUnitAction::None: // (a)
			break;
		case EUnitAction::Heal: // (b)
			Unit->Heal();
			break;
		default: //(c)
			const TArray<FMovement> Path = TileMap->FindPath(UnitInfo.Pos2D,
			                                                 CalculateBestPositionForUnit(Unit, UnitAction),
			                                                 UnitInfo.Type, UnitInfo.BaseMovementPoints,
			                                                 UnitInfo.MovementPoints);
			Unit->AssignPath(Path);
			break;
		}
	}
}

void ACMainAI::ManageSettlementsProduction()
{
}

//--------------------------------------------------------------------------------------------------------------------//

void ACMainAI::TurnStarted()
{
	// Se trata de inicializar la faccion que gestiona el controlador, si su referencia no es valida
	if (!PawnFaction) PawnFaction = Cast<APawnFaction>(GetPawn());

	// Si la faccion no es valida, no se hace nada
	if (PawnFaction)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) AI Turn Started"), PawnFaction->GetIndex()))

		// Primero: se gestionan los movimientos de las unidades de la faccion
		ManageUnits();

		// Segundo: se gestiona la produccion de los asentamientos
		ManageSettlementsProduction();
	}

	// Se finaliza el turno actual
	TurnFinished();
}

void ACMainAI::TurnFinished()
{
	// Si la faccion no es valida, no se hace nada
	if (PawnFaction)
	{
		// Se finaliza el turno de la faccion
		PawnFaction->TurnEnded();

		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) AI Turn Finished"), PawnFaction->GetIndex()))
	}

	// Se llama al metodo para gestionar la siguiente faccion
	if (const AMMain* MainMode = Cast<AMMain>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		MainMode->NextTurn();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void ACMainAI::BeginPlay()
{
	Super::BeginPlay();

	// Se inicializa la instancia del mapa
	TileMap = Cast<AActorTileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorTileMap::StaticClass()));
}

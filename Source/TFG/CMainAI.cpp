// Fill out your copyright notice in the Description page of Project Settings.


#include "CMainAI.h"

#include "ActorCivilUnit.h"
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

	// Se inicializa la casilla para establecer asentamientos
	BestTileForSettlement = TTuple<FIntPoint, float>(FIntPoint(0), -1);

	// Se inicializa el numero de unidades en movimiento
	UnitsMoving = 0;
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

FIntPoint ACMainAI::GetClosestTilePos(const FIntPoint& Pos, TArray<FIntPoint>& SettlementOwnedTiles) const
{
	FIntPoint ClosestPos = Pos;

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

void ACMainAI::UpdateTilesValue()
{
	// Se invalida el valor de atractivo de la mejor casilla actual para actualizarlo
	BestTileForSettlement.Value = -1;

	// Se recorren todas las casillas del mapa
	for (int32 Row = 0; Row < TileMap->GetSize().X; ++Row)
	{
		for (int32 Col = 0; Col < TileMap->GetSize().Y; ++Col)
		{
			// Posicion de la casilla actual
			const FIntPoint Pos = FIntPoint(Row, Col);

			// Si el diccionario no contiene la casilla, se inserta
			if (!TilesValue.Contains(Pos)) TilesValue.Add(Pos, 0.0);

			// Si no se puede establecer un asentamiento, se le da un valor invalido
			if (!TileMap->CanSetSettlementAtPos(Pos)) TilesValue[Pos] = -1.0;
			else
			{
				// Se obtiene una referencia al valor para modificarlo
				float& TileValue = TilesValue[Pos];

				// Se obtiene el asentamiento mas cercano para que el modificador de mas valor a las casillas mas
				// cercanas con el objetivo de establecer los asentamientos relativamente cerca unos de otros
				const AActorSettlement* Settlement = GetClosestOwnedSettlementFromPos(Pos);
				const FIntPoint SettlementPos = Settlement ? Settlement->GetPos() : FIntPoint(-1);

				// Se hace que el valor sumado sea inversamente proporcional a la distancia que se encuentre
				// el asentamiento
				TileValue += SettlementPos != -1
					             ? 10.0 / (ULibraryTileMap::GetDistanceToElement(Pos, SettlementPos) - 3.0)
					             : 0.0;

				// Si la casilla tiene un recurso, se aumenta el valor
				if (TileMap->TileHasResource(Pos)) TileValue += 5.0;
			}

			// Si el valor de atractivo es mejor que el de la casilla actual, se actualiza
			if (TilesValue[Pos] > BestTileForSettlement.Value)
			{
				BestTileForSettlement = TTuple<FIntPoint, float>(Pos, TilesValue[Pos]);
			}
		}
	}
}

bool ACMainAI::IsSettlementNeeded() const
{
	// Se obtiene el numero de asentamientos propios
	const int32 NumSettlements = PawnFaction->GetNumSettlements();

	// Si no hay asentamientos, se obliga a establecerlo
	if (NumSettlements == 0) return true;

	// Se obtiene el estado para acceder a las facciones
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		// Se procesan todas las facciones
		for (const auto Faction : State->GetFactions())
		{
			// Si es la faccion actual, se omite
			if (Faction.Key == PawnFaction->GetIndex()) continue;

			// Se verifica el numero de asentamientos para determinar si es necesario establecer uno nuevo
			if (NumSettlements <= Faction.Value->GetNumSettlements()) return true;
		}
	}

	return false;
}

FIntPoint ACMainAI::CalculateBestPosForSettlement()
{
	// Si no hay asentamientos, se establece en la posicion actual
	if (PawnFaction->GetNumSettlements() == 0) return -1;

	// Se actualizan los valores de atractivo de las casillas y se devuelve el mejor
	UpdateTilesValue();

	return BestTileForSettlement.Key;
}

FIntPoint ACMainAI::GetClosestResourceToGatherPos(const FIntPoint& Pos) const
{
	FIntPoint ClosestResource = Pos;

	// Variable que almacena la distancia al recurso mas cercano
	int32 MinDistance = 999;

	// Se obtienen todos los recursos y se procesan
	TMap<EResource, FResourceCollection> Resources = PawnFaction->GetMonetaryResources();
	Resources.Append(PawnFaction->GetStrategicResources());

	for (const auto Resource : Resources)
	{
		// Se procesan todos los recursos en posesion
		for (const auto ResourcePos : Resource.Value.Tiles)
		{
			// Si el recurso ha sido recolectado, se omite
			if (TileMap->IsResourceGathered(ResourcePos)) continue;

			// Se obtiene la distancia y, si es menor, se actualiza las variables
			const int32 Distance = ULibraryTileMap::GetDistanceToElement(Pos, ResourcePos);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestResource = ResourcePos;
			}
		}
	}

	return ClosestResource;
}

//--------------------------------------------------------------------------------------------------------------------//

TSet<FIntPoint> ACMainAI::GetEnemyOrAllyLocationInRange(const FIntPoint& Pos, const int32 Range,
                                                        const bool GetEnemy) const
{
	// Coleccion de casillas con enemigos
	TSet<FIntPoint> ElementsLocation = TSet<FIntPoint>();

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
	// Se obtiene el asentamiento mas cercano
	return GetClosestSettlementFromPos(Pos, PawnFaction->GetSettlements());
}

FIntPoint ACMainAI::GetFarthestPosFromEnemies(const FIntPoint& Pos, const int32 Range) const
{
	FIntPoint FarthestPos = Pos;

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

FIntPoint ACMainAI::GetClosestPosToAlly(const FIntPoint& Pos) const
{
	FIntPoint ClosestPos = Pos;

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

FIntPoint ACMainAI::GetClosestEnemyTilePos(const FIntPoint& Pos) const
{
	// Se obtienen las casillas en posesion del asentamiento enemigo mas cercano
	TArray<FIntPoint> SettlementOwnedTiles = GetClosestEnemySettlementFromPos(Pos)->GetOwnedTiles();

	// Se obtiene la casilla mas cercana sin ocupar y se devuelve
	return GetClosestTilePos(Pos, SettlementOwnedTiles);
}

FIntPoint ACMainAI::GetClosestAllyTilePos(const FIntPoint& Pos) const
{
	// Se obtienen las casillas en posesion del asentamiento propio mas cercano
	TArray<FIntPoint> SettlementOwnedTiles = GetClosestOwnedSettlementFromPos(Pos)->GetOwnedTiles();

	// Se obtiene la casilla mas cercana sin ocupar y se devuelve
	return GetClosestTilePos(Pos, SettlementOwnedTiles);
}

FIntPoint ACMainAI::CalculateBestPosForUnit(const FUnitInfo& UnitInfo, const EUnitAction UnitAction) const
{
	TArray<FString> ParsedEnum;
	UEnum::GetValueAsString(UnitAction).ParseIntoArray(ParsedEnum, TEXT(":"), false);
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("CalculatingPos - %s"), *ParsedEnum.Last()))

	// Si se debe mover hacia un enemigo, se obtiene el mas cercano
	if (UnitAction == EUnitAction::MoveTowardsEnemy)
	{
		return ULibraryTileMap::GetClosestElementFromPos(UnitInfo.Pos2D, EnemiesLocation);
	}
	// Si debe huir de un enemigo, se obtiene la posicion mas alejada en conjunto de todos los enemigos
	if (UnitAction == EUnitAction::MoveAwayFromEnemy)
	{
		return GetFarthestPosFromEnemies(UnitInfo.Pos2D, UnitInfo.MovementPoints);
	}
	// Si se debe mover hacia un aliado, se obtiene el mas cercano y la posicion mas cercana a este que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsAlly)
	{
		return GetClosestPosToAlly(UnitInfo.Pos2D);
	}
	// Si se debe mover hacia una casilla enemiga, se obtiene la mas cercana que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsEnemyTiles)
	{
		return GetClosestEnemyTilePos(UnitInfo.Pos2D);
	}
	// Si se debe mover hacia una casilla aliada, se obtiene la mas cercana que no este ocupada
	if (UnitAction == EUnitAction::MoveTowardsAllyTiles)
	{
		return GetClosestAllyTilePos(UnitInfo.Pos2D);
	}
	// Si debe explorar, se obtiene aleatoriamente una casilla dentro del alcance de movimiento
	if (UnitAction == EUnitAction::MoveAround)
	{
		// Se obtienen las casillas al alcance de la unidad y se procesan para eliminar las ocupadas
		TArray<FIntPoint> TilesInRange = TileMap->GetTilesWithinRange(UnitInfo.Pos2D, UnitInfo.MovementPoints, true);
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
	return UnitInfo.Pos2D;
}

//--------------------------------------------------------------------------------------------------------------------//

EUnitType ACMainAI::CalculateBestUnitTypeToProduce() const
{
	// Se decide la unidad a producir en funcion de si se quiere defender, atacar o realizar otra accion
	EUnitType BestUnitType;

	const int32 Value = FMath::RandRange(1, 3);
	if (Value == 1) BestUnitType = EUnitType::Infantry;
	else if (Value == 2) BestUnitType = EUnitType::Armoured;
	else BestUnitType = EUnitType::AntiTank;

	return BestUnitType;
}

//--------------------------------------------------------------------------------------------------------------------//

void ACMainAI::ManageCivilUnit(AActorUnit* Unit)
{
	// Se realiza el cast para poder acceder a los metodos de la clase
	AActorCivilUnit* CivilUnit = Cast<AActorCivilUnit>(Unit);

	// Si no se ha podido realizar el cast, se finaliza
	if (!CivilUnit) return;

	// Se obtienen los atributos de la unidad para poder usarlos durante el proceso
	const FUnitInfo UnitInfo = CivilUnit->GetInfo();
	// const FVector2D Health = FVector2D(Unit->GetBaseHealthPoints(), Unit->GetHealthPoints());

	// Se calcula el porcentaje de salud
	// const float HealthPercentage = Health.Y / Health.X;

	// SEGUNDO:
	//		(1) Si tiene un camino asignado: se verifica que el destino es aun valido
	//		(2) Si no tiene un camino:
	//				(2.1) Se decide si debe crear un asentamiento o recolectar un recurso
	//				(2.2) Se mueve la unidad a la casilla

	if (UnitInfo.Path.Num() != 0) // (1)
	{
		// Se verifica el estado de la unidad civil
		switch (CivilUnit->GetCivilUnitState())
		{
		case ECivilUnitState::GatheringResource:
			// Se verifica que se pueda recolectar el recurso
			if (!TileMap->CanGatherResourceAtPos(UnitInfo.Path.Last().Pos2D))
			{
				CivilUnit->SetCivilUnitState(ECivilUnitState::None);
			}
			break;
		case ECivilUnitState::SettingSettlement:
			// Se verifica que se pueda crear el asentamiento
			if (!TileMap->CanSetSettlementAtPos(UnitInfo.Path.Last().Pos2D))
			{
				CivilUnit->SetCivilUnitState(ECivilUnitState::None);
			}
			break;
		default:
			CivilUnit->SetCivilUnitState(ECivilUnitState::None);
			break;
		}
	}

	if (UnitInfo.Path.Num() == 0 && CivilUnit->GetCivilUnitState() == ECivilUnitState::None) // (2)
	{
		FIntPoint NewPos;

		// Se determina si se debe crear un nuevo asentamiento
		if (CivilUnit->CanSetSettlement() && IsSettlementNeeded()) // (2.1)
		{
			// Se obtiene la posicion y se establece el estado de la unidad
			NewPos = CalculateBestPosForSettlement();
			CivilUnit->SetCivilUnitState(ECivilUnitState::SettingSettlement);

			// Si la posicion dada no es valida, quiere decir que se debe establecer en la actual
			if (NewPos == -1) NewPos = UnitInfo.Pos2D;
		}
		else
		{
			// Se obtiene la posicion y se establece el estado de la unidad
			NewPos = GetClosestResourceToGatherPos(UnitInfo.Pos2D);
			CivilUnit->SetCivilUnitState(ECivilUnitState::GatheringResource);
		}

		// (2.2)
		// Si no se ha calculado ninguna posicion y la unidad se encuentra sobre un asentamiento, se mueve a otra
		if (NewPos == UnitInfo.Pos2D && TileMap->GetSettlementsPos().Contains(NewPos))
		{
			NewPos = CalculateBestPosForUnit(UnitInfo, EUnitAction::MoveAround);
		}

		// Se calcula el camino a la nueva posicion y se asigna a la unidad
		const TArray<FMovement> Path = TileMap->FindPath(UnitInfo.Pos2D, NewPos, UnitInfo.Type,
		                                                 UnitInfo.BaseMovementPoints, UnitInfo.MovementPoints);
		Unit->AssignPath(Path);
	}
}

void ACMainAI::ManageMilitaryUnit(AActorUnit* Unit) const
{
	// Se obtienen los atributos de la unidad para poder usarlos durante el proceso
	const FUnitInfo UnitInfo = Unit->GetInfo();
	const FVector2D Health = FVector2D(Unit->GetBaseHealthPoints(), Unit->GetHealthPoints());

	// Se calcula el porcentaje de salud
	const float HealthPercentage = Health.Y / Health.X;

	// SEGUNDO:
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
		UE_LOG(LogTemp, Log, TEXT("ENEMIES"))

		// En caso de que la unidad tenga un camino asignado a una casilla propia, no se hace nada
		if (UnitInfo.Path.Num() == 0 || !TileMap->IsTileMine(UnitInfo.Path.Last().Pos2D))
		{
			UE_LOG(LogTemp, Log, TEXT("Changing Path"))

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
		UE_LOG(LogTemp, Log, TEXT("NOT ENEMIES"))

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
		UE_LOG(LogTemp, Log, TEXT("Action - Doing nothing"))
		break;
	case EUnitAction::Heal: // (b)
		UE_LOG(LogTemp, Log, TEXT("Action - Healing"))

		Unit->Heal();
		break;
	default: //(c)
		UE_LOG(LogTemp, Log, TEXT("Action - Moving"))
		UE_LOG(LogTemp, Log, TEXT("Calculating NewPos"))

		const FIntPoint NewPos = CalculateBestPosForUnit(UnitInfo, UnitAction);

		UE_LOG(LogTemp, Log, TEXT("Moving Unit"))

		const TArray<FMovement> Path = TileMap->FindPath(UnitInfo.Pos2D, NewPos, UnitInfo.Type,
		                                                 UnitInfo.BaseMovementPoints, UnitInfo.MovementPoints);

		UE_LOG(LogTemp, Log, TEXT("Assigning new path"))

		Unit->AssignPath(Path);
		break;
	}
}

void ACMainAI::ManageUnits()
{
	// Si la faccion o la instancia del mapa no es valida, no se hace nada
	if (!PawnFaction || !TileMap) return;

	const TArray<AActorUnit*> Units = PawnFaction->GetUnits();
	for (const auto Unit : Units)
	{
		// Se obtienen los atributos de la unidad para poder usarlos durante el proceso
		const FUnitInfo UnitInfo = Unit->GetInfo();

		UE_LOG(LogTemp, Log, TEXT("Calculating EnemiesLocation"))

		// PRIMERO: se comprueba si hay enemigos cerca
		EnemiesLocation = GetEnemyOrAllyLocationInRange(UnitInfo.Pos2D, UnitInfo.BaseMovementPoints, true);

		UE_LOG(LogTemp, Log, TEXT("Calculating AlliesLocation"))

		// Tambien se obtienen las posiciones de las unidades aliadas cercanas
		AlliesLocation = GetEnemyOrAllyLocationInRange(UnitInfo.Pos2D, UnitInfo.BaseMovementPoints, false);

		// Segun el tipo de unidad, se realiza una operacion u otra
		if (UnitInfo.Type != EUnitType::None)
		{
			if (UnitInfo.Type == EUnitType::Civil) ManageCivilUnit(Unit);
			else ManageMilitaryUnit(Unit);
		}
	}
}

void ACMainAI::ManageSettlementsProduction() const
{
	// Si la faccion o la instancia del mapa no es valida, no se hace nada
	if (!PawnFaction || !TileMap) return;

	// Se obtienen los asentamientos de la faccion
	const TArray<AActorSettlement*> Settlements = PawnFaction->GetSettlements();
	const TArray<int32> IdleSettlements = PawnFaction->GetIdleSettlements();
	for (const auto Settlement : IdleSettlements)
	{
		// Si la referencia al asentamiento no es valida, se omite
		if (!Settlements[Settlement]) continue;

		// Se obtiene el tipo de unidad que se tiene que producir
		const EUnitType UnitType = IsSettlementNeeded() ? EUnitType::Civil : CalculateBestUnitTypeToProduce();

		// Se llama al evento que establece la produccion del asentamiento
		OnUnitProductionSelection.Broadcast(Settlements[Settlement], UnitType);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

bool ACMainAI::CanFinishTurn() const
{
	// Se verifica que la faccion sea valida
	if (!PawnFaction) return false;

	// Se obtienen y procesan todas las unidades
	const TArray<AActorUnit*> Units = PawnFaction->GetUnits();
	for (const auto Unit : Units)
	{
		// Si una unidad esta en movimiento, se devuelve
		if (Unit->GetIsMoving()) return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------//

void ACMainAI::BeginPlay()
{
	Super::BeginPlay();

	// Se inicializa la instancia del mapa
	TileMap = Cast<AActorTileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorTileMap::StaticClass()));

	// Si inicializa el diccionario que almacena los valores de atractivo de las casillas
	for (int32 Row = 0; Row < TileMap->GetSize().X; ++Row)
	{
		for (int32 Col = 0; Col < TileMap->GetSize().Y; ++Col)
		{
			TilesValue.Add(FIntPoint(Row, Col), 0.0);
		}
	}
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

void ACMainAI::TurnFinished() const
{
	// Si la faccion no es valida, no se hace nada
	if (PawnFaction)
	{
		// Se finaliza el turno de la faccion
		PawnFaction->TurnEnded();

		// Se obtienen las unidades
		const TArray<AActorUnit*> Units = PawnFaction->GetUnits();

		// Se verifica si las unidades civiles aun pueden realizar acciones
		for (const auto Unit : Units)
		{
			// Se omiten las unidades que no sean civiles
			if (Unit->GetType() != EUnitType::Civil) continue;

			// Se realiza el cast para poder acceder a los metodos de la clase
			AActorCivilUnit* CivilUnit = Cast<AActorCivilUnit>(Unit);

			// Se verifica que haya completado el camino y le queden puntos de movimiento
			if (CivilUnit && CivilUnit->GetPath().Num() == 0 && CivilUnit->GetMovementPoints() > 0)
			{
				// Se verifica si puede recolectar un recurso
				if (CivilUnit->GetCivilUnitState() == ECivilUnitState::GatheringResource &&
					TileMap->CanGatherResourceAtPos(CivilUnit->GetPos()))
				{
					CivilUnit->GatherResource();
				}
				// Se verifica si puede establecer un asentamiento
				else if (CivilUnit->GetCivilUnitState() == ECivilUnitState::SettingSettlement &&
					CivilUnit->CanSetSettlement() && TileMap->CanSetSettlementAtPos(CivilUnit->GetPos()))
				{
					CivilUnit->CreateSettlement();
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) AI Turn Finished"), PawnFaction->GetIndex()))
	}

	// Se llama al metodo para gestionar el final del turno
	OnTurnFinished.Broadcast();
}

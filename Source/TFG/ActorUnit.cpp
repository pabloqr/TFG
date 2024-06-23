// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorUnit.h"

#include "LibraryTileMap.h"


// Sets default values
AActorUnit::AActorUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	State = EUnitState::WaitingForOrders;

	BaseMovementPoints = MovementPoints = 2;
	VisibilityPoints = 2;

	ProductionCost = 100.0;
	MaintenanceCost = 2.0;

	TileMap = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::UpdatePathCosts()
{
	// Se verifica que la unidad se haya movido en el turno
	if (PathCompleted.Num() > 0)
	{
		// Se obtiene el coste actual y se actualizan el resto de costes
		const int32 Cost = PathCompleted.Last().TotalCost;
		for (int32 i = 0; i < Path.Num(); ++i) Path[i].TotalCost -= Cost;
	}

	// Se actualiza el numero de turnos en alcanzar cada una de las casillas
	UpdatePathTurns();
}

void AActorUnit::UpdatePathTurns()
{
	ULibraryTileMap::UpdatePathTurns(Path, BaseMovementPoints, MovementPoints);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::UpdatePosition(const FMovement& Move)
{
	if (Move.Pos2D != Pos2D)
	{
		// Se actualiza la posicion y los puntos de movimiento restantes
		SetPos(Move.Pos2D);
		MovementPoints -= Move.MovementCost;

		// Se actualiza el estado de la unidad
		if (MovementPoints == 0) State = EUnitState::NoMovementPoints;
		else if (Path.Num() == 0) State = EUnitState::WaitingForOrders;
		else State = EUnitState::FollowingPath;
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::AssignPath(const TArray<FMovement>& NewPath)
{
	// Se limpian los valores almacenados del camino previo
	Path.Empty();
	PathCompleted.Empty();
	
	if (NewPath.Num() > 0)
	{
		// Se actualiza el camino a seguir
		Path.Append(NewPath);
		if (Path[0].Pos2D == Pos2D) Path.RemoveAt(0);
	
		// Se establece el estado de la unidad
		if (MovementPoints > 0) State = EUnitState::FollowingPath;

		// Se actualiza el numero de turnos para alcanzar las casillas del camino
		UpdatePathTurns();
	}
}

void AActorUnit::UpdatePath()
{
	if (Path.Num() > 0 && TileMap)
	{
		// Se recalcula el camino
		const TArray<FMovement> NewPath = TileMap->FindPath(Pos2D, Path.Last().Pos2D, BaseMovementPoints, MovementPoints);
		AssignPath(NewPath);
	}
}

void AActorUnit::RemovePath()
{
	// Se limpian los valores almacenados del camino previo
	Path.Empty();
	PathCompleted.Empty();

	// Se establece el estado de la unidad
	if (MovementPoints > 0) State = EUnitState::WaitingForOrders;
	else State = EUnitState::NoMovementPoints;
}

void AActorUnit::ContinuePath()
{
	// Se limpian las casillas completadas en el turno previo
	PathCompleted.Empty();
	
	// Se almacena la posicion antes del movimiento
	const FIntPoint PrevPos = Pos2D;

	// Se realizan todos los movimientos posibles en el turno
	while (State == EUnitState::FollowingPath) MoveUnit();

	// Se llama al evento para que se actualicen los datos en el resto de actores
	OnUnitMoved.Broadcast(PrevPos, PathCompleted);
}

void AActorUnit::MoveUnit()
{
	// Se comprueba que haya pasos en el camino a seguir
	if (Path.Num() > 0)
	{
		// Se obtiene el siguiente movimiento
		const FMovement Move = Path[0];
	
		// Se comprueba que la unidad tenga puntos de movimiento suficientes
		if (MovementPoints >= Move.MovementCost)
		{
			// Se elimina la entrada correspondiente al movimiento actual
			PathCompleted.Add(Move);
			Path.RemoveAt(0);

			// Se actualiza la posicion
			UpdatePosition(Move);
			
			return;
		}

		// Si no se puede mover, pero le quedan puntos de movimiento, se actualiza el estado
		State = EUnitState::WaitingForNextTurn;
	}
}

void AActorUnit::RestoreMovement()
{
	MovementPoints = BaseMovementPoints;
}

//--------------------------------------------------------------------------------------------------------------------//

EUnitState AActorUnit::TurnStarted()
{
	// Se restablecen los puntos de movimiento al comienzo del turno
	RestoreMovement();

	// Se actualiza el estado de la unidad
	if (State != EUnitState::Sleeping)
	{
		if (Path.Num() > 0) State = EUnitState::FollowingPath;
		else State = EUnitState::WaitingForOrders;
	}

	return State;
}

void AActorUnit::TurnEnded()
{
	// Se actualizan los costes del camino
	UpdatePath();

	// Se continua el camino si tiene uno asignado
	ContinuePath();
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void AActorUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

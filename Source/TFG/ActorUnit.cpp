// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorUnit.h"

#include "ActorTileMap.h"
#include "LibraryTileMap.h"


// Sets default values
AActorUnit::AActorUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Info = FUnitInfo();
	TileMap = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::UpdatePathCosts()
{
	// Se verifica que la unidad se haya movido en el turno
	if (Info.PathCompleted.Num() > 0)
	{
		// Se obtiene el coste actual y se actualizan el resto de costes
		const int32 Cost = Info.PathCompleted.Last().TotalCost;
		for (int32 i = 0; i < Info.Path.Num(); ++i) Info.Path[i].TotalCost -= Cost;
	}

	// Se actualiza el numero de turnos en alcanzar cada una de las casillas
	UpdatePathTurns();
}

void AActorUnit::UpdatePathTurns()
{
	ULibraryTileMap::UpdatePathTurns(Info.Path, Info.BaseMovementPoints, Info.MovementPoints);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::UpdatePosition(const FMovement& Move)
{
	if (Move.Pos2D != Info.Pos2D)
	{
		// Se actualiza la posicion y los puntos de movimiento restantes
		Info.Pos2D = Move.Pos2D;
		Info.MovementPoints -= Move.MovementCost;

		// Se actualiza el estado de la unidad
		if (Info.MovementPoints == 0) SetState(EUnitState::NoMovementPoints);
		else if (Info.Path.Num() == 0) SetState(EUnitState::WaitingForOrders);
		else SetState(EUnitState::FollowingPath);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::SetState(const EUnitState State)
{
	// Se actualiza el estado
	Info.State = State;

	// Se llama al evento tras actualizar el estado
	OnUnitStateChanged.Broadcast(this, Info.State);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::AssignPath(const TArray<FMovement>& NewPath)
{
	// Se limpian los valores almacenados del camino previo
	Info.Path.Empty();

	if (NewPath.Num() > 0)
	{
		// Se actualiza el camino a seguir
		Info.Path.Append(NewPath);
		if (Info.Path[0].Pos2D == Info.Pos2D) Info.Path.RemoveAt(0);

		// Se establece el estado de la unidad
		if (Info.MovementPoints > 0) SetState(EUnitState::FollowingPath);

		// Se actualiza el numero de turnos para alcanzar las casillas del camino
		UpdatePathTurns();
	}
}

void AActorUnit::UpdatePath()
{
	if (Info.Path.Num() > 0 && TileMap)
	{
		// Se recalcula el camino
		const TArray<FMovement> NewPath = TileMap->FindPath(Info.Pos2D, Info.Path.Last().Pos2D, Info.Type,
		                                                    Info.BaseMovementPoints, Info.MovementPoints);

		// Se asigna al camino de la unidad
		AssignPath(NewPath);
	}
}

void AActorUnit::RemovePath()
{
	// Se limpian los valores almacenados del camino previo
	Info.Path.Empty();
	Info.PathCompleted.Empty();

	// Se establece el estado de la unidad
	if (Info.MovementPoints > 0) SetState(EUnitState::WaitingForOrders);
	else SetState(EUnitState::NoMovementPoints);
}

void AActorUnit::ContinuePath()
{
	// Se limpian las casillas completadas en el turno previo
	Info.PathCompleted.Empty();

	// Se almacena la posicion antes del movimiento
	const FIntPoint PrevPos = Info.Pos2D;

	// Se realizan todos los movimientos posibles en el turno
	while (Info.State == EUnitState::FollowingPath) MoveUnit();

	// Se llama al evento para que se actualicen los datos en el resto de actores
	OnUnitMoved.Broadcast(PrevPos);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::Heal()
{
	// Se actualiza el estado
	SetState(EUnitState::Healing);
}

void AActorUnit::SkipTurn()
{
	// Se actualiza el estado
	SetState(EUnitState::WaitingForNextTurn);
}

void AActorUnit::MoveUnit()
{
	// Se comprueba que haya pasos en el camino a seguir
	if (Info.Path.Num() > 0)
	{
		// Se obtiene el siguiente movimiento
		const FMovement Move = Info.Path[0];

		// Se comprueba que la unidad tenga puntos de movimiento suficientes
		if (Info.MovementPoints >= Move.MovementCost)
		{
			// Se elimina la entrada correspondiente al movimiento actual
			Info.PathCompleted.Add(Move);
			Info.Path.RemoveAt(0);

			// Se actualiza la posicion
			UpdatePosition(Move);

			return;
		}

		// Si no se puede mover, pero le quedan puntos de movimiento, se actualiza el estado
		SetState(EUnitState::WaitingForNextTurn);
	}
}

void AActorUnit::RestoreMovement()
{
	Info.MovementPoints = Info.BaseMovementPoints;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::ApplyDamage(const float Damage)
{
	Super::ApplyDamage(Damage);

	// Si no quedan puntos de vida, se llama al evento que gestiona la destruccion de la unidad
	if (DamageableInfo.HealthPoints <= 0.0) OnUnitDestroyed.Broadcast(this);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::TurnStarted()
{
	// Se cura la unidad si es posible
	if (Info.IsHealing)
	{
		// Se actualizan los puntos de vida
		DamageableInfo.HealthPoints += 10.0f;

		// Se actualiza el estado de curacion
		Info.IsHealing = false;

		if (DamageableInfo.HealthPoints >= DamageableInfo.BaseHealthPoints)
		{
			// Si la unidad tiene todos los puntos de vida, se obliga a que no superen a los base
			DamageableInfo.HealthPoints = DamageableInfo.BaseHealthPoints;

			// Si se estaba curando, se cambia el estado de la unidad
			if (Info.State == EUnitState::Healing) SetState(EUnitState::WaitingForOrders);
		}
	}

	// Se restablecen los puntos de movimiento al comienzo del turno
	RestoreMovement();

	// Se actualiza el estado de la unidad
	if (Info.State != EUnitState::Sleeping && Info.State != EUnitState::Healing)
	{
		// Se recalcula el camino para tener en cuenta cualquier unidad ajena que haya podido interponerse
		if (Info.Path.Num() > 0) UpdatePath();
		else if (Info.State != EUnitState::WaitingForOrders) SetState(EUnitState::WaitingForOrders);
	}
}

void AActorUnit::TurnEnded()
{
	// Se recalcula el camino para tener en cuenta cualquier unidad propia que haya podido interponerse
	UpdatePath();

	// Se continua el camino si tiene uno asignado
	ContinuePath();

	// Si la unidad no ha efectuado ninguna accion en este turno y no tiene su salud completa, se cura
	if (Info.MovementPoints == Info.BaseMovementPoints && DamageableInfo.HealthPoints < DamageableInfo.BaseHealthPoints)
	{
		// Se actualiza el estado de curacion
		Info.IsHealing = true;
	}
}

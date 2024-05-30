// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorUnit.h"

#include "Components/TimelineComponent.h"


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

// Called when the game starts or when spawned
void AActorUnit::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::AssignPath(const TArray<FMovement>& NewPath)
{
	Path = NewPath;
	State = EUnitState::FollowingPath;
}

void AActorUnit::ContinuePath()
{
	while (State == EUnitState::FollowingPath && MoveUnit()) {}
}

bool AActorUnit::MoveUnit()
{
	// Se comprueba que haya pasos en el camino a seguir
	if (Path.Num() > 0)
	{
		// Se obtiene el siguiente movimiento
		const FMovement Move = Path[0];
	
		// Se comprueba que la unidad tenga puntos de movimiento suficientes
		if (MovementPoints >= Move.MovementCost)
		{
			// Se elimina la entrada del camino a seguir
			Path.RemoveAt(0);
			
			// Se almacena la posicion antes del movimiento
			const FIntPoint PrevPos = Pos2D;
		
			// Se actualiza la posicion y los puntos de movimiento restantes
			SetPos(Move.Pos2D);
			MovementPoints -= Move.MovementCost;

			// Se actualiza el estado de la unidad
			if (MovementPoints == 0) State = EUnitState::NoMovementPoints;
			else if (Path.Num() == 0) State = EUnitState::WaitingForOrders;

			// Se llama al evento para que se actualicen los datos en el resto de actores
			OnUnitMoved.Broadcast(PrevPos, Pos2D);
			return true;
		}
	}

	return false;
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
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void AActorUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

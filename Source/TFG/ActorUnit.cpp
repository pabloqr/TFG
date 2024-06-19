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

void AActorUnit::UpdatePathCosts()
{
	// Se verifica que la unidad se haya movido en el turno
	if (PathCompleted.Num() == 0) return;

	UE_LOG(LogTemp, Log, TEXT("Updating Unit Path Costs"))
	
	// Se obtiene el coste actual y se actualizan el resto de costes
	const int32 Cost = PathCompleted[PathCompleted.Num()-1].TotalCost;
	for (int32 i = 0; i < Path.Num(); ++i) Path[i].TotalCost -= Cost;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void AActorUnit::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorUnit::RemovePath()
{
	// Se limpian los valores almacenados del camino previo
	Path.Empty();
	PathCompleted.Empty();

	// Se establece el estado de la unidad
	if (MovementPoints > 0) State = EUnitState::WaitingForOrders;
	else State = EUnitState::NoMovementPoints;
}

void AActorUnit::AssignPath(const TArray<FMovement>& NewPath)
{
	// Se limpian los valores almacenados del camino previo
	Path.Empty();
	PathCompleted.Empty();
	
	// Se actualiza el camino a seguir
	Path.Append(NewPath);
	
	// Se establece el estado de la unidad
	if (MovementPoints > 0) State = EUnitState::FollowingPath;
}

void AActorUnit::ContinuePath()
{
	// Se limpian las casillas completadas en el turno previo
	PathCompleted.Empty();

	// Se realizan todos los movimientos posibles en el turno
	while (State == EUnitState::FollowingPath)
	{
		MoveUnit();
	}
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
			// Se elimina la entrada correspondiente al movimiento actual
			PathCompleted.Add(Move);
			Path.RemoveAt(0);
			
			// Se almacena la posicion antes del movimiento
			const FIntPoint PrevPos = Pos2D;
		
			// Se actualiza la posicion y los puntos de movimiento restantes
			SetPos(Move.Pos2D);
			MovementPoints -= Move.MovementCost;

			// Se actualiza el estado de la unidad
			if (MovementPoints == 0) State = EUnitState::NoMovementPoints;
			else if (Path.Num() == 0) State = EUnitState::WaitingForOrders;
			else State = EUnitState::WaitingForNextTurn;

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
	// Se actualizan los costes del camino
	UpdatePathCosts();
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void AActorUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSettlement.h"

#include "FProductionElement.h"


AActorSettlement::AActorSettlement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	State = ESettlementState::None;
	MoneyYield = 0;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::SetInitialOwnedTiles()
{
	OwnedTiles = GetNeighbors();
	OwnedTiles.AddUnique(Pos2D);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::AddToProductionQueue(const TSubclassOf<AActorUnit> UnitToProduce)
{
	// Se anade el elemento a la cola de produccion
	ProductionQueue.Add(FProductionElement(UnitToProduce, 5));

	// Se actualiza el estado
	State = ESettlementState::Producing;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, State);
}

void AActorSettlement::RemoveFromProduction(const int32 Index)
{
	// Se verifica que el indice sea valido
	if (0 <= Index && Index < ProductionQueue.Num())
	{
		// Se elimina el elemento de la lista
		ProductionQueue.RemoveAt(Index);

		// Se actualiza el estado
		State = ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;
		
		// Se llama al evento tras actualizar el estado
		OnSettlementStateChanged.Broadcast(this, State);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

ESettlementState AActorSettlement::GetState() const
{
	return State;
}

float AActorSettlement::GetMoneyYield() const
{
	return MoneyYield;
}

void AActorSettlement::SetState(const ESettlementState SettlementState)
{
	State = SettlementState;
}

void AActorSettlement::SetMoneyYield(const float Yield)
{
	MoneyYield = Yield;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::TurnStarted()
{
	// Se comprueba si hay elementos en produccion
	if (ProductionQueue.Num() > 0)
	{
		// Se actualiza el coste de produccion
		ProductionQueue[0].ProductionCost -= 1;
		
		// Se comprueba si se han completado todos los turnos para producir la unidad
		if (ProductionQueue[0].ProductionCost <= 0)
		{
			// Se obtiene la unidad a generar
			const TSubclassOf<AActorUnit> UnitToSpawn = ProductionQueue[0].Unit;
		
			// Se elimina la unidad de la cola de produccion
			ProductionQueue.RemoveAt(0);

			// Se llama al evento para generar la unidad en el mapa
			OnUnitProduced.Broadcast(Pos2D, UnitToSpawn);
		}
	}

	// Se actualiza el estado
	State = ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, State);
}

void AActorSettlement::TurnEnded()
{
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


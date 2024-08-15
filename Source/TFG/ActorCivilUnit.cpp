// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorCivilUnit.h"


// Sets default values
AActorCivilUnit::AActorCivilUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializan los atributos de la unidad
	NumActions = 2;

	CivilUnitState = ECivilUnitState::None;
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorCivilUnit::CanSetSettlement() const
{
	return Info.MovementPoints > 0 && NumActions == 2;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorCivilUnit::GatherResource()
{
	// Si no tiene puntos de movimiento suficientes no se hace nada
	if (!Info.MovementPoints) return;

	// Se actualizan los puntos de movimiento de la unidad
	Info.MovementPoints = 0;

	// Se realiza la llamada al evento para que se gestione la recoleccion del recurso
	OnResourceGathered.Broadcast(Info.Pos2D);

	// Se verifica si quedan acciones
	if (--NumActions <= 0)
	{
		// Se llama al evento para gestionar la destruccion de la unidad
		OnUnitDestroyed.Broadcast(this);
	}
}

void AActorCivilUnit::CreateSettlement()
{
	// Solo se crea el asentamiento si la unidad posee todas las acciones y tiene puntos de movimiento disponibles
	if (Info.MovementPoints && NumActions == 2)
	{
		// Se llama al evento para gestionar la creacion del asentamiento
		OnSettlementCreated.Broadcast(Info.Pos2D);

		// Se llama al evento para gestionar la destruccion de la unidad
		OnUnitDestroyed.Broadcast(this);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void AActorCivilUnit::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void AActorCivilUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

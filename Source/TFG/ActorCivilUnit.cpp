// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorCivilUnit.h"


// Sets default values
AActorCivilUnit::AActorCivilUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializan los atributos de la unidad
	NumActions = 2;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorCivilUnit::GatherResource()
{
	// Se realiza la llamada al evento para que se gestione la recoleccion del recurso
	OnResourceGathered.Broadcast(Info.Pos2D);

	// Si no quedan acciones, se destruye el actor
	if (--NumActions <= 0) Destroy();
}

void AActorCivilUnit::CreateSettlement()
{
	// Solo se crea el asentamiento si la unidad posee todas las acciones
	if (NumActions == 2)
	{
		// Se llama al evento para gestionar la creacion del asentamiento
		OnSettlementCreated.Broadcast(Info.Pos2D);

		// Se destruye el actor
		Destroy();
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

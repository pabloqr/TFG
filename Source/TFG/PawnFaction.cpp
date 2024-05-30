// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnFaction.h"

#include "ActorUnit.h"

// Sets default values
APawnFaction::APawnFaction()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Money = 0.0;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void APawnFaction::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::TurnStarted()
{
	// Se inicia el turno de las unidades de la faccion y se realiza su clasificacion para que puedan ser
	// manejadas de forma correcta
	//
	// Las unidades "automaticas" son aquellas con un camino asociado que, en principio, deben seguir avanzando por su
	// cuenta hasta que lo completen. El resto de unidades son aquellas que requieren una orden
	//
	// Ademas, se actualiza el dinero y balance de la faccion teniendo en cuenta el coste de manteimiento de
	// las unidades

	// Se reinicia el balance de dinero
	MoneyBalance = 0.0;

	// Se vacian los arrays para actualizarlos
	AutomaticUnits.Empty();
	ManualUnits.Empty();

	// Se procesan todas las unidades
	for (int32 i = 0; i < Units.Num(); ++i)
	{
		AActorUnit* Unit = Units[i];

		// Se inicia el turno de la unidad
		const EUnitState UnitState = Unit->TurnStarted();

		// Se clasifica la unidad
		if (UnitState == EUnitState::FollowingPath) AutomaticUnits.AddUnique(i);
		else if (UnitState == EUnitState::WaitingForOrders) ManualUnits.AddUnique(i);

		// Se actualiza el balance de dinero con el coste de matenimiento de la unidad actual
		MoneyBalance -= Unit->GetMaintenanceCost();
	}

	// Se inicia el turno de los asentamientos de la faccion y se verifica si requieren seleccionar un nuevo
	// elemento a producir
	for (int32 i = 0; i < Settlements.Num(); ++i)
	{
		
	}

	// Se actualiza el balance de dinero teniendo en cuenta el rendimiento de los recursos
	for (const AActorResource* Resource : Resources)
	{
		if (Resource->GetType() == EResourceType::Monetary) MoneyBalance += Resource->GetQuantity();
	}

	// Se actualiza el dinero de la faccion teniendo en cuenta:
	//		* El coste de mantenimiento de todas las unidades
	//		* El rendimiento de los asentamientos
	//		* El rendimiento de los recursos
	Money += MoneyBalance;
}

void APawnFaction::TurnEnded()
{
	// Se procesan todas las unidades que estan siguiendo un camino
	for (int32 i = 0; i < AutomaticUnits.Num(); ++i)
	{
		// Se obtiene el indice de la unidad que se esta procesando
		const int32 UnitIndex = AutomaticUnits[i];

		// Se realiza el siguiente movimiento en el camino a seguir
		AActorUnit* Unit = Units[UnitIndex];
		Unit->ContinuePath();

		const EUnitState UnitState = Unit->GetState();
		if (UnitState != EUnitState::FollowingPath)
		{
			AutomaticUnits.RemoveAt(i);
			if (UnitState == EUnitState::WaitingForOrders) ManualUnits.AddUnique(UnitIndex);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void APawnFaction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APawnFaction::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


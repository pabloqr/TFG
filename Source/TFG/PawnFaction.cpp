// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnFaction.h"

#include "ActorUnit.h"

// Sets default values
APawnFaction::APawnFaction()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	for (AActorUnit* Unit : Units)
	{
		Unit->TurnStarted();
	}
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
			AutomaticUnits.RemoveAt(UnitIndex);
			if (UnitState == EUnitState::WaitingForOrders) ManualUnits.Add(UnitIndex);
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


// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnFaction.h"

#include "ActorSettlement.h"
#include "ActorUnit.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APawnFaction::APawnFaction()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Money = MoneyBalance = 0.0;
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::OnUnitStateUpdated(const AActorUnit* Unit, const EUnitState& State)
{
	// Se busca la unidad en el array de unidades de la faccion
	int32 Index = -1;
	for (int32 i = 0; i < Units.Num() && Index == -1; ++i)
	{
		if (Units[i] == Unit) Index = i;
	}

	// Si no se encuentra la unidad, no se ejecuta nada mas
	if (Index == -1) return;

	// Se comprueba si la unidad esta siguiendo un camino o espera ordenes
	if (State == EUnitState::FollowingPath && !AutomaticUnits.Contains(Index))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (ManualUnits.Contains(Index)) ManualUnits.Remove(Index);
		AutomaticUnits.AddUnique(Index);
	}
	else if (State == EUnitState::WaitingForOrders && !ManualUnits.Contains(Index))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (AutomaticUnits.Contains(Index)) ManualUnits.Remove(Index);
		AutomaticUnits.AddUnique(Index);
	}
	else
	{
		// Si no tiene ninguno de los estados anteriores, se elimina de cualquiera de las dos listas
		if (AutomaticUnits.Contains(Index)) AutomaticUnits.Remove(Index);
		else if (ManualUnits.Contains(Index)) ManualUnits.Remove(Index);
	}
}

void APawnFaction::OnSettlementStateUpdated(const AActorSettlement* Settlement, const ESettlementState& State)
{
	// Se busca el asentamiento en el array de asentamientos de la faccion
	int32 Index = -1;
	for (int32 i = 0; i < Settlements.Num() && Index == -1; ++i)
	{
		if (Settlements[i] == Settlement) Index = i;
	}

	// Si no se encuentra el asentamiento, no se ejecuta nada mas
	if (Index == -1) return;
	
	// Se actualiza la lista de acuerdo al estado del asentamiento
	if (State == ESettlementState::SelectProduction)
	{
		if (!IdleSettlements.Contains(Index)) IdleSettlements.AddUnique(Index);
	}
	else if (IdleSettlements.Contains(Index)) IdleSettlements.Remove(Index);
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

bool APawnFaction::HasElement(const AActorDamageableElement* Element) const
{
	// Se comprueba si es una unidad y, si lo es, se verifica si es de esta faccion
	const AActorUnit* Unit = Cast<AActorUnit>(Element);
	if (Unit) return Units.Contains(Unit);

	// Se comprueba si es un asentamiento y, si lo es, se verifica si es de esta faccion
	const AActorSettlement* Settlement = Cast<AActorSettlement>(Element);
	if (Settlement) return Settlements.Contains(Settlement);

	return false;
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
		// Se obtiene la unidad
		AActorUnit* Unit = Units[i];

		// Se inicia el turno de la unidad
		Unit->TurnStarted();

		// Se actualiza el balance de dinero con el coste de matenimiento de la unidad actual
		MoneyBalance -= Unit->GetMaintenanceCost();
	}

	// Se inicia el turno de los asentamientos de la faccion y se verifica si requieren seleccionar un nuevo
	// elemento a producir
	for (int32 i = 0; i < Settlements.Num(); ++i)
	{
		// Se obtiene el asentamiento
		AActorSettlement* Settlement = Settlements[i];

		// Se inicia el turno del asentamiento
		Settlement->TurnStarted();

		// Se actualiza el balance de dinero con el rendimiento del asentamiento
		MoneyBalance += Settlement->GetMoneyYield();
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
	// Se procesan todas las unidades
	for (int32 i = 0; i < Units.Num(); ++i)
	{
		// Se obtiene la unidad
		AActorUnit* Unit = Units[i];
		
		// Se finaliza el turno de la unidad
		Unit->TurnEnded();
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


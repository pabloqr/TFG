// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnFaction.h"

#include "ActorSettlement.h"
#include "LibraryDataTables.h"

// Sets default values
APawnFaction::APawnFaction()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializa el indice de la faccion
	Index = 0;

	// Se inicializa la fuerza militar de la faccion
	MilitaryStrength = 0.0;

	// Se inicializa el dinero y el balance
	Money = MoneyBalance = 0.0;

	// Se inicializan los diccionarios de recursos
	MonetaryResources = TMap<EResource, FResourceCollection>({
		{
			EResource::Diamond,
			FResourceCollection(FResource(EResource::Diamond, EResourceType::Monetary, 0), TArray<FIntPoint>())
		},
		{
			EResource::Gold,
			FResourceCollection(FResource(EResource::Gold, EResourceType::Monetary, 0), TArray<FIntPoint>())
		},
		{
			EResource::Copper,
			FResourceCollection(FResource(EResource::Copper, EResourceType::Monetary, 0), TArray<FIntPoint>())
		}
	});
	StrategicResources = TMap<EResource, FResourceCollection>({
		{
			EResource::Aluminium,
			FResourceCollection(FResource(EResource::Aluminium, EResourceType::Strategic, 0), TArray<FIntPoint>())
		},
		{
			EResource::Coal,
			FResourceCollection(FResource(EResource::Coal, EResourceType::Strategic, 0), TArray<FIntPoint>())
		},
		{
			EResource::Oil,
			FResourceCollection(FResource(EResource::Oil, EResourceType::Strategic, 0), TArray<FIntPoint>())
		}
	});
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::OnUnitStateUpdated(const AActorUnit* Unit, const EUnitState State)
{
	// Se busca la unidad en el array de unidades de la faccion
	int32 UnitIndex = -1;
	for (int32 i = 0; i < Units.Num() && UnitIndex == -1; ++i)
	{
		if (Units[i] == Unit) UnitIndex = i;
	}

	// Si no se encuentra la unidad, no se ejecuta nada mas
	if (UnitIndex == -1) return;

	// Se comprueba si la unidad esta siguiendo un camino o espera ordenes
	if (State == EUnitState::FollowingPath && !AutomaticUnits.Contains(UnitIndex))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (ManualUnits.Contains(UnitIndex)) ManualUnits.Remove(UnitIndex);
		AutomaticUnits.AddUnique(UnitIndex);
	}
	else if (State == EUnitState::WaitingForOrders && !ManualUnits.Contains(UnitIndex))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (AutomaticUnits.Contains(UnitIndex)) ManualUnits.Remove(UnitIndex);
		AutomaticUnits.AddUnique(UnitIndex);
	}
	else
	{
		// Si no tiene ninguno de los estados anteriores, se elimina de cualquiera de las dos listas
		if (AutomaticUnits.Contains(UnitIndex)) AutomaticUnits.Remove(UnitIndex);
		else if (ManualUnits.Contains(UnitIndex)) ManualUnits.Remove(UnitIndex);
	}
}

void APawnFaction::OnSettlementStateUpdated(const AActorSettlement* Settlement, const ESettlementState State)
{
	// Se busca el asentamiento en el array de asentamientos de la faccion
	int32 SettlementIndex = -1;
	for (int32 i = 0; i < Settlements.Num() && SettlementIndex == -1; ++i)
	{
		if (Settlements[i] == Settlement) SettlementIndex = i;
	}

	// Si no se encuentra el asentamiento, no se ejecuta nada mas
	if (SettlementIndex == -1) return;

	// Se actualiza la lista de acuerdo al estado del asentamiento
	if (State == ESettlementState::SelectProduction)
	{
		if (!IdleSettlements.Contains(SettlementIndex)) IdleSettlements.AddUnique(SettlementIndex);
	}
	else if (IdleSettlements.Contains(SettlementIndex)) IdleSettlements.Remove(SettlementIndex);
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

bool APawnFaction::CanProduceUnit(const UDataTable* DataTable, const EUnitType UnitType) const
{
	// Se obtiene la informacion de la unidad
	const FUnitData UnitData = ULibraryDataTables::GetUnitDataFromType(DataTable, UnitType);
	if (StrategicResources.Contains(UnitData.RequiredResource.Resource))
	{
		// Se devuelve si hay recursos suficientes
		return StrategicResources[UnitData.RequiredResource.Resource].GatheredResource.Quantity >=
			UnitData.RequiredResource.Quantity;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddSettlement(AActorSettlement* Settlement)
{
	// Se establece la faccion actual como propietaria del asentamiento
	Settlement->SetFactionOwner(Index);

	// Se anade el asentamiento a la lista
	Settlements.AddUnique(Settlement);

	// Se actualiza su estado
	OnSettlementStateUpdated(Settlement, ESettlementState::SelectProduction);
}

void APawnFaction::RemoveSettlement(AActorSettlement* Settlement)
{
	// Se elimina el asentamiento de la lista
	if (Settlements.Contains(Settlement)) Settlements.Remove(Settlement);
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddUnit(AActorUnit* Unit)
{
	// Se establece la faccion actual como propietaria de la unidad
	Unit->SetFactionOwner(Index);

	// Se anade la unidad a la lista
	Units.AddUnique(Unit);

	// Se actualiza su estado
	OnUnitStateUpdated(Unit, EUnitState::WaitingForOrders);
}

void APawnFaction::RemoveUnit(AActorUnit* Unit)
{
	// Se verifica que la unidad sea de esta faccion
	const int32 UnitIndex = Units.Find(Unit);
	if (UnitIndex != INDEX_NONE)
	{
		// Se elimina el indice de las listas de unidades
		if (ManualUnits.Contains(UnitIndex)) ManualUnits.Remove(UnitIndex);
		if (AutomaticUnits.Contains(UnitIndex)) AutomaticUnits.Remove(UnitIndex);

		// Se elimina la unidad de la lista
		Units.Remove(Unit);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::OwnResource(const EResource Resource, const FIntPoint& Pos)
{
	// Se anade la posicion del recurso a la lista correspondiente para indicar que esta dentro de las fronteras
	if (MonetaryResources.Contains(Resource)) MonetaryResources[Resource].Tiles.Add(Pos);
	else if (StrategicResources.Contains(Resource)) StrategicResources[Resource].Tiles.Add(Pos);
}

void APawnFaction::DisownResource(const EResource Resource, const FIntPoint& Pos)
{
	// Se elimina la posicion del recurso a la lista correspondiente para indicar que ya no esta dentro de las fronteras
	if (MonetaryResources.Contains(Resource)) MonetaryResources[Resource].Tiles.Remove(Pos);
	else if (StrategicResources.Contains(Resource)) StrategicResources[Resource].Tiles.Remove(Pos);
}

void APawnFaction::AddResource(const FResource& Resource, const FIntPoint& Pos)
{
	// Se obtiene el diccionario a modificar dependiendo del tipo de recurso dado
	TMap<EResource, FResourceCollection>& Resources = Resource.Type == EResourceType::Monetary
		                                                  ? MonetaryResources
		                                                  : StrategicResources;

	// Se comprueba si el recurso ya lo posee la unidad y se actualiza su cantidad, en caso contrario,
	// no se hace nada, ya que no es un tipo de recurso valido
	if (Resources.Contains(Resource.Resource) && Resources[Resource.Resource].Tiles.Contains(Pos))
	{
		Resources[Resource.Resource].GatheredResource.Quantity += Resource.Quantity;
	}
}

void APawnFaction::RemoveResource(const FResource& Resource, const FIntPoint& Pos)
{
	// Se obtiene el diccionario a modificar dependiendo del tipo de recurso dado
	TMap<EResource, FResourceCollection>& Resources = Resource.Type == EResourceType::Monetary
		                                                  ? MonetaryResources
		                                                  : StrategicResources;

	// Se comprueba si el recurso ya lo posee la unidad y se actualiza su cantidad, en caso contrario,
	// no se hace nada, ya que no es un tipo de recurso valido
	if (Resources.Contains(Resource.Resource) && Resources[Resource.Resource].Tiles.Contains(Pos))
	{
		// Se hace que la cantidad siempre sea mayor o igual a 0
		Resources[Resource.Resource].GatheredResource.Quantity =
			FMath::Max(Resources[Resource.Resource].GatheredResource.Quantity - Resource.Quantity, 0);
	}
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

	// Se reinicia el balance de dinero y la fuerza militar de la faccion
	MoneyBalance = 0.0;
	MilitaryStrength = 0.0;

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

		// Se actualiza la fuerza militar de la faccion con la fuerza de ataque de la unidad actualF
		MilitaryStrength += Unit->GetAttackPoints();
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
	for (const auto Resource : MonetaryResources)
	{
		MoneyBalance += Resource.Value.GatheredResource.Quantity;
	}

	// Se actualiza el dinero de la faccion teniendo en cuenta:
	//		* El coste de mantenimiento de todas las unidades
	//		* El rendimiento de los asentamientos
	//		* El rendimiento de los recursos
	Money += MoneyBalance;

	// Se llama al evento para actualizar todos los parametros del juego
	OnTurnStarted.Broadcast();
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

void APawnFaction::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

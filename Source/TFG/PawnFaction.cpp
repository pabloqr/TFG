// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnFaction.h"

#include "ActorSettlement.h"
#include "LibraryDataTables.h"

// Sets default values
APawnFaction::APawnFaction()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializa la informacion de la faccion para modificarla a continuacion
	Info = FFactionInfo();

	// Se inicializa el indice de la faccion
	Info.Index = 0;

	// Se inicializa la fuerza militar de la faccion
	Info.MilitaryStrength = 0.0;

	// Se inicializa el dinero y el balance
	Info.Money = 200.0;
	Info.MoneyBalance = 0.0;

	// Se inicializan los diccionarios de recursos
	Info.MonetaryResources = TMap<EResource, FResourceCollection>({
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
	Info.StrategicResources = TMap<EResource, FResourceCollection>({
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

	// Se inicializa el diccionario de relaciones diplomaticas
	Info.FactionsWithDiplomaticRelationship = TMap<EDiplomaticRelationship, FFactionsSet>({
		{EDiplomaticRelationship::AtWar, FFactionsSet()},
		{EDiplomaticRelationship::Neutral, FFactionsSet()},
		{EDiplomaticRelationship::Ally, FFactionsSet()},
	});
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::UpdateFactionDiplomaticRelationship(const int32 Faction, const EDiplomaticRelationship Relationship)
{
	// Se verifica que la faccion existe
	if (!Info.KnownFactions.Contains(Faction)) return;

	// Se actualiza el diccionario de relaciones diplomaticas y facciones
	Info.FactionsWithDiplomaticRelationship[Info.KnownFactions[Faction].Relationship].Factions.Remove(Faction);
	Info.FactionsWithDiplomaticRelationship[Relationship].Factions.Add(Faction);

	// Se actualiza la relacion diplomatica de la faccion dada
	Info.KnownFactions[Faction].Relationship = Relationship;
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::OnUnitStateUpdated(const AActorUnit* Unit, const EUnitState State)
{
	// Se busca la unidad en el array de unidades de la faccion
	int32 UnitIndex = -1;
	for (int32 i = 0; i < Info.Units.Num() && UnitIndex == -1; ++i)
	{
		if (Info.Units[i] == Unit) UnitIndex = i;
	}

	// Si no se encuentra la unidad, no se ejecuta nada mas
	if (UnitIndex == -1) return;

	// Se comprueba si la unidad esta siguiendo un camino o espera ordenes
	if (State == EUnitState::FollowingPath && !Info.AutomaticUnits.Contains(UnitIndex))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (Info.ManualUnits.Contains(UnitIndex)) Info.ManualUnits.Remove(UnitIndex);
		Info.AutomaticUnits.AddUnique(UnitIndex);
	}
	else if (State == EUnitState::WaitingForOrders && !Info.ManualUnits.Contains(UnitIndex))
	{
		// Se comprueba que no este en la otra lista y se anade a la correcta
		if (Info.AutomaticUnits.Contains(UnitIndex)) Info.ManualUnits.Remove(UnitIndex);
		Info.AutomaticUnits.AddUnique(UnitIndex);
	}
	else
	{
		// Si no tiene ninguno de los estados anteriores, se elimina de cualquiera de las dos listas
		if (Info.AutomaticUnits.Contains(UnitIndex)) Info.AutomaticUnits.Remove(UnitIndex);
		else if (Info.ManualUnits.Contains(UnitIndex)) Info.ManualUnits.Remove(UnitIndex);
	}
}

void APawnFaction::OnSettlementStateUpdated(const AActorSettlement* Settlement, const ESettlementState State)
{
	// Se busca el asentamiento en el array de asentamientos de la faccion
	int32 SettlementIndex = -1;
	for (int32 i = 0; i < Info.Settlements.Num() && SettlementIndex == -1; ++i)
	{
		if (Info.Settlements[i] == Settlement) SettlementIndex = i;
	}

	// Si no se encuentra el asentamiento, no se ejecuta nada mas
	if (SettlementIndex == -1) return;

	// Se actualiza la lista de acuerdo al estado del asentamiento
	if (State == ESettlementState::SelectProduction)
	{
		if (!Info.IdleSettlements.Contains(SettlementIndex)) Info.IdleSettlements.AddUnique(SettlementIndex);
	}
	else if (Info.IdleSettlements.Contains(SettlementIndex)) Info.IdleSettlements.Remove(SettlementIndex);
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddKnowFaction(const int32 Faction, const FOpponentFactionInfo& FactionInfo)
{
	// Se anade la faccion y su informacion
	Info.KnownFactions.Add(Faction, FactionInfo);
	// Se anade la faccion al estado diplomatico neutral
	Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Neutral].Factions.Add(Faction);
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

bool APawnFaction::CanProduceUnit(const UDataTable* DataTable, const EUnitType UnitType,
                                  const bool CalculateProbability) const
{
	// Se verifica que la referencia a la tabla sea correcta y que se solicite la comprobacion de una unidad militar
	if (!DataTable || UnitType == EUnitType::Civil) return true;

	// Se obtiene la informacion de la unidad
	const FUnitData UnitData = ULibraryDataTables::GetUnitDataFromType(DataTable, UnitType);
	if (Info.StrategicResources.Contains(UnitData.RequiredResource.Resource))
	{
		// Se calcula una probabilidad de produccion dependiendo de la cantidad de dinero y balance disponibles
		float Probability = CalculateProbability || Info.Money <= 0.0 ? 0.0 : 1.0;
		if (CalculateProbability && Info.Money > 0.0)
		{
			Probability = Info.Money /
				(Info.Money + (12.0 + UnitData.MaintenanceCost) * FMath::Abs(Info.MoneyBalance) + 1.0);
		}

		// Se verifica si hay dinero suficiente
		const bool HasMoney = Probability >= 0.5;
		// Se verifica si hay recursos suficientes
		const bool HasResources = Info.StrategicResources[UnitData.RequiredResource.Resource].GatheredResource.Quantity
			>= UnitData.RequiredResource.Quantity;

		return HasResources && HasMoney;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddMoney(const int32 Amount)
{
	Info.Money += Amount;
}

void APawnFaction::RemoveMoney(const int32 Amount)
{
	Info.Money -= Amount;
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddSettlement(AActorSettlement* Settlement)
{
	// Se establece la faccion actual como propietaria del asentamiento
	Settlement->SetFactionOwner(Info.Index);

	// Se anade el asentamiento a la lista
	Info.Settlements.AddUnique(Settlement);

	// Se actualiza el balance de dinero
	Info.MoneyBalance += Settlement->GetMoneyYield();

	// Se llama al evento para actualizar la interfaz
	OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);

	// Se actualiza su estado
	OnSettlementStateUpdated(Settlement, ESettlementState::SelectProduction);
}

void APawnFaction::RemoveSettlement(AActorSettlement* Settlement)
{
	// Se elimina el asentamiento de la lista
	if (Info.Settlements.Contains(Settlement))
	{
		Info.Settlements.Remove(Settlement);

		// Se actualiza el balance de dinero
		Info.MoneyBalance -= Settlement->GetMoneyYield();

		// Se llama al evento para actualizar la interfaz
		OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::AddUnit(AActorUnit* Unit)
{
	// Se establece la faccion actual como propietaria de la unidad
	Unit->SetFactionOwner(Info.Index);

	// Se anade la unidad a la lista
	Info.Units.AddUnique(Unit);

	if (Unit->GetType() == EUnitType::Civil) Info.CivilUnits.AddUnique(Info.Units.Find(Unit));
	else
	{
		Info.MilitaryUnits.AddUnique(Info.Units.Find(Unit));

		// Se actualiza la fuerza militar para que se tenga en cuenta la unidad anadida
		Info.MilitaryStrength += Unit->GetStrengthPoints();
	}

	// Se actualiza el balance de dinero para que tenga en cuenta la unidad anadida
	Info.MoneyBalance -= Unit->GetMaintenanceCost();

	// Se llama al evento para actualizar la interfaz
	OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);

	// Se actualiza su estado
	OnUnitStateUpdated(Unit, EUnitState::WaitingForOrders);
}

void APawnFaction::RemoveUnit(AActorUnit* Unit)
{
	// Se verifica que la unidad sea de esta faccion
	const int32 UnitIndex = Info.Units.Find(Unit);
	if (UnitIndex != INDEX_NONE)
	{
		// Se elimina el indice de las listas de unidades
		if (Info.ManualUnits.Contains(UnitIndex)) Info.ManualUnits.Remove(UnitIndex);
		if (Info.AutomaticUnits.Contains(UnitIndex)) Info.AutomaticUnits.Remove(UnitIndex);

		// Se elimina la unidad de la lista
		Info.Units.Remove(Unit);

		// Se actualiza la fuerza militar para que no se tenga en cuenta la unidad eliminada
		if (Unit->GetType() != EUnitType::Civil) Info.MilitaryStrength -= Unit->GetStrengthPoints();

		// Se actualiza el balance de dinero para que no tenga en cuenta la unidad eliminada
		Info.MoneyBalance += Unit->GetMaintenanceCost();

		// Se llama al evento para actualizar la interfaz
		OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::OwnResource(const EResource Resource, const FIntPoint& Pos)
{
	// Se anade la posicion del recurso a la lista correspondiente para indicar que esta dentro de las fronteras
	if (Info.MonetaryResources.Contains(Resource)) Info.MonetaryResources[Resource].Tiles.Add(Pos);
	else if (Info.StrategicResources.Contains(Resource)) Info.StrategicResources[Resource].Tiles.Add(Pos);
}

void APawnFaction::DisownResource(const EResource Resource, const FIntPoint& Pos)
{
	// Se elimina la posicion del recurso a la lista correspondiente para indicar que ya no esta dentro de las fronteras
	if (Info.MonetaryResources.Contains(Resource)) Info.MonetaryResources[Resource].Tiles.Remove(Pos);
	else if (Info.StrategicResources.Contains(Resource)) Info.StrategicResources[Resource].Tiles.Remove(Pos);
}

void APawnFaction::AddResource(const bool CheckPos, const FResource& Resource, const FIntPoint& Pos)
{
	// Se obtiene el diccionario a modificar dependiendo del tipo de recurso dado
	TMap<EResource, FResourceCollection>& Resources = Resource.Type == EResourceType::Monetary
		                                                  ? Info.MonetaryResources
		                                                  : Info.StrategicResources;

	// Se comprueba si el recurso ya lo posee la faccion y se actualiza su cantidad, en caso contrario,
	// no se hace nada, ya que no es un tipo de recurso valido
	if (Resources.Contains(Resource.Resource) && (!CheckPos || Resources[Resource.Resource].Tiles.Contains(Pos)))
	{
		Resources[Resource.Resource].GatheredResource.Quantity += Resource.Quantity;

		// Si el recurso es monetario, se actualiza el balance de dinero
		if (Resource.Type == EResourceType::Monetary)
		{
			Info.MoneyBalance += Resource.Quantity;

			// Se llama al evento para actualizar la interfaz
			OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);
		}

		// Se llama al evento para actualizar la interfaz
		OnResourceQuantityUpdated.Broadcast(Resource.Resource, Resources[Resource.Resource].GatheredResource.Quantity);
	}
}

void APawnFaction::RemoveResource(const bool CheckPos, const FResource& Resource, const FIntPoint& Pos)
{
	// Se obtiene el diccionario a modificar dependiendo del tipo de recurso dado
	TMap<EResource, FResourceCollection>& Resources = Resource.Type == EResourceType::Monetary
		                                                  ? Info.MonetaryResources
		                                                  : Info.StrategicResources;

	// Se comprueba si el recurso ya lo posee la faccion y se actualiza su cantidad, en caso contrario,
	// no se hace nada, ya que no es un tipo de recurso valido
	if (Resources.Contains(Resource.Resource) && (!CheckPos || Resources[Resource.Resource].Tiles.Contains(Pos)))
	{
		// Se hace que la cantidad siempre sea mayor o igual a 0
		Resources[Resource.Resource].GatheredResource.Quantity =
			FMath::Max(Resources[Resource.Resource].GatheredResource.Quantity - Resource.Quantity, 0);

		// Si el recurso es monetario, se actualiza el balance de dinero
		if (Resource.Type == EResourceType::Monetary)
		{
			Info.MoneyBalance -= Resource.Quantity;

			// Se llama al evento para actualizar la interfaz
			OnMoneyBalanceUpdated.Broadcast(Info.MoneyBalance);
		}

		// Se llama al evento para actualizar la interfaz
		OnResourceQuantityUpdated.Broadcast(Resource.Resource, Resources[Resource.Resource].GatheredResource.Quantity);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::UpdateKnownFactionsInfo(const TMap<int32, float>& FactionsStrength,
                                           const TMap<int32, FRelationshipInfo>& CurrentWars,
                                           const TMap<int32, FRelationshipInfo>& CurrentAlliances)
{
	// Se obtienen todos los indices de las facciones conocidas y se procesan
	TArray<int32> FactionsIndex;
	Info.KnownFactions.GetKeys(FactionsIndex);
	for (const auto FactionIndex : FactionsIndex)
	{
		// Si la faccion no se encuentra en la lista significa que ya no esta en juego, por lo que se elimina
		// y se omite
		if (!FactionsStrength.Contains(FactionIndex))
		{
			// Se elimina de la coleccion de facciones conocidas
			Info.KnownFactions.Remove(FactionIndex);

			// Se elimina del diccionario de relaciones diplomaticas
			int32 Removed = Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::AtWar]
			                .Factions.Remove(FactionIndex);
			if (Removed == 0)
			{
				Removed = Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Neutral]
				          .Factions.Remove(FactionIndex);
			}
			if (Removed == 0)
			{
				Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Ally].Factions.Remove(FactionIndex);
			}

			continue;
		}

		// Si no existe la entrada para la faccion actual, se anade
		if (!Info.KnownFactions.Contains(FactionIndex)) AddKnowFaction(FactionIndex, FOpponentFactionInfo());

		// Se actualiza la fuerza militar de la faccion
		Info.KnownFactions[FactionIndex].MilitaryStrength = FactionsStrength[FactionIndex];

		// Se actualizan los datos sobre la guerra
		if (CurrentWars.Contains(FactionIndex))
		{
			// Se actualiza la relacion
			Info.KnownFactions[FactionIndex].Relationship = EDiplomaticRelationship::AtWar;

			// Se actualiza la informacion sobre la guerra
			Info.KnownFactions[FactionIndex].WarInfo.Score = CurrentWars[FactionIndex].Score;
			Info.KnownFactions[FactionIndex].WarInfo.Turns = CurrentWars[FactionIndex].Turns;
		}
		else
		{
			Info.KnownFactions[FactionIndex].WarInfo.Turns += 1;
		}

		// Se actualizan los datos sobre la alianza
		if (CurrentAlliances.Contains(FactionIndex))
		{
			// Se actualiza la relacion
			Info.KnownFactions[FactionIndex].Relationship = EDiplomaticRelationship::Ally;

			// Se actualiza la informacion sobre la alianza
			Info.KnownFactions[FactionIndex].AllianceInfo.Score = CurrentAlliances[FactionIndex].Score;
			Info.KnownFactions[FactionIndex].AllianceInfo.Turns = CurrentAlliances[FactionIndex].Turns;
		}
		else
		{
			Info.KnownFactions[FactionIndex].AllianceInfo.Turns += 1;
		}
	}
}

void APawnFaction::DeclareWarOnFaction(const int32 Faction)
{
	// Se actualiza la relacion diplomatica
	UpdateFactionDiplomaticRelationship(Faction, EDiplomaticRelationship::AtWar);

	// Se actualiza la informacion sobre la guerra entre las dos facciones
	Info.KnownFactions[Faction].WarInfo.Score = 0.0;
	Info.KnownFactions[Faction].WarInfo.Turns = 0;

	ResetWarPetitionTurns(Faction);
}

void APawnFaction::ResetWarPetitionTurns(const int32 Faction)
{
	Info.KnownFactions[Faction].WarInfo.PetitionTurns = 0;
}

void APawnFaction::MakePeaceWithFaction(const int32 Faction)
{
	// Se actualiza la relacion diplomatica
	UpdateFactionDiplomaticRelationship(Faction, EDiplomaticRelationship::Neutral);

	// Se actualiza la informacion sobre la guerra entre las dos facciones
	Info.KnownFactions[Faction].WarInfo.Score = 0.0;
	Info.KnownFactions[Faction].WarInfo.Turns = 0;
}

void APawnFaction::MakeAllianceWithFaction(const int32 Faction)
{
	// Se actualiza la relacion diplomatica
	UpdateFactionDiplomaticRelationship(Faction, EDiplomaticRelationship::Ally);

	// Se actualiza la informacion sobre la alianza entre las dos facciones
	Info.KnownFactions[Faction].AllianceInfo.Score = 0.0;
	Info.KnownFactions[Faction].AllianceInfo.Turns = 0;
}

void APawnFaction::ResetAlliancePetitionTurns(const int32 Faction)
{
	Info.KnownFactions[Faction].WarInfo.PetitionTurns = 0;
}

void APawnFaction::BreakAllianceWithFaction(const int32 Faction)
{
	// Se actualiza la relacion diplomatica
	UpdateFactionDiplomaticRelationship(Faction, EDiplomaticRelationship::Neutral);

	// Se actualiza la informacion sobre la alianza entre las dos facciones
	Info.KnownFactions[Faction].AllianceInfo.Score = 0.0;
	Info.KnownFactions[Faction].AllianceInfo.Turns = 0;

	ResetAlliancePetitionTurns(Faction);
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
	Info.MoneyBalance = 0.0;
	Info.MilitaryStrength = 0.0;

	// Se vacian los arrays para actualizarlos
	Info.AutomaticUnits.Empty();
	Info.ManualUnits.Empty();

	// Se procesan todas las unidades
	for (int32 i = 0; i < Info.Units.Num(); ++i)
	{
		// Se obtiene la unidad
		AActorUnit* Unit = Info.Units[i];

		// Se inicia el turno de la unidad
		Unit->TurnStarted();

		// Se actualiza el balance de dinero con el coste de matenimiento de la unidad actual
		Info.MoneyBalance -= Unit->GetMaintenanceCost();

		// Se actualiza la fuerza militar de la faccion con la fuerza de ataque de la unidad actualF
		if (Unit->GetType() != EUnitType::Civil) Info.MilitaryStrength += Unit->GetStrengthPoints();
	}

	// Se inicia el turno de los asentamientos de la faccion y se verifica si requieren seleccionar un nuevo
	// elemento a producir
	for (int32 i = 0; i < Info.Settlements.Num(); ++i)
	{
		// Se obtiene el asentamiento
		AActorSettlement* Settlement = Info.Settlements[i];

		// Se inicia el turno del asentamiento
		Settlement->TurnStarted();

		// Se actualiza el balance de dinero con el rendimiento del asentamiento
		Info.MoneyBalance += Settlement->GetMoneyYield();
	}

	// Se actualiza el balance de dinero teniendo en cuenta el rendimiento de los recursos
	for (const auto Resource : Info.MonetaryResources)
	{
		Info.MoneyBalance += Resource.Value.GatheredResource.Quantity;
	}

	// Se actualiza el dinero de la faccion teniendo en cuenta:
	//		* El coste de mantenimiento de todas las unidades
	//		* El rendimiento de los asentamientos
	//		* El rendimiento de los recursos
	Info.Money += Info.MoneyBalance;

	// Se llama al evento para actualizar todos los parametros del juego
	OnTurnStarted.Broadcast();
}

void APawnFaction::TurnEnded()
{
	// Se procesan todas las unidades y asentamientos
	for (int32 i = 0; i < Info.Units.Num() || i < Info.Settlements.Num(); ++i)
	{
		// Se trata de obtener la unidad
		if (i < Info.Units.Num())
		{
			// Se obtiene la unidad
			AActorUnit* Unit = Info.Units[i];

			// Se finaliza el turno de la unidad
			Unit->TurnEnded();
		}

		// Se trata de obtener el asentamiento
		if (i < Info.Settlements.Num())
		{
			// Se obtiene el asentamiento
			AActorSettlement* Settlement = Info.Settlements[i];

			// Se finaliza el turno del asentamiento
			Settlement->TurnEnded();
		}
	}
}

void APawnFaction::CleanInfoAfterLosing()
{
	// Se restablece la fuerza militar
	Info.MilitaryStrength = 0.0;

	// Se restablece el dinero
	Info.Money = 0.0;
	Info.MoneyBalance = 0.0;

	// Se restablecen los asentamientos
	Info.IdleSettlements.Empty();

	// Se restablecen las unidades
	Info.CivilUnits.Empty();
	Info.MilitaryUnits.Empty();
	Info.ManualUnits.Empty();
	Info.AutomaticUnits.Empty();

	// Se restablecen los recursos monetarios
	for (const auto Resource : Info.MonetaryResources)
	{
		Info.MonetaryResources[Resource.Key].GatheredResource.Quantity = 0.0;
		Info.MonetaryResources[Resource.Key].Tiles.Empty();
	}

	// Se restablecen los recursos estrategicos
	for (const auto Resource : Info.StrategicResources)
	{
		Info.StrategicResources[Resource.Key].GatheredResource.Quantity = 0.0;
		Info.StrategicResources[Resource.Key].Tiles.Empty();
	}

	// Se restablecen las facciones conocidas
	Info.KnownFactions.Empty();

	// Se restablecen las relaciones con las facciones
	for (const auto Relationship : Info.FactionsWithDiplomaticRelationship)
	{
		Info.FactionsWithDiplomaticRelationship[Relationship.Key].Factions.Empty();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void APawnFaction::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

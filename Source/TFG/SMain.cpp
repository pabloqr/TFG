// Fill out your copyright notice in the Description page of Project Settings.


#include "SMain.h"

TMap<FFactionsPair, FWarInfo> ASMain::GetCurrentWarsForFaction(const int32 Faction) const
{
	TMap<FFactionsPair, FWarInfo> CurrentWarsForFaction = TMap<FFactionsPair, FWarInfo>();

	if (FactionsAlive.Contains(Faction) && Factions.Contains(Faction))
	{
		for (const auto War : CurrentWars)
		{
			if (War.Key.Contains(Faction)) CurrentWarsForFaction.Add(War.Key, War.Value);
		}
	}

	return CurrentWarsForFaction;
}

//--------------------------------------------------------------------------------------------------------------------//

void ASMain::AddFaction(APawnFaction* Faction)
{
	// Se obtiene el indice de la faccion
	const int32 Index = Faction->GetIndex();

	// Se anade la entrada o se actualiza si existe
	if (Factions.Contains(Index)) Factions[Index] = Faction;
	else Factions.Add(Index, Faction);

	// Solo se anade a la lista si no esta en ella
	if (!FactionsAlive.Contains(Index)) FactionsAlive.Add(Index);
}

int32 ASMain::SetFactionDead(const int32 Index)
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return 0;

	// Se elimina la faccion y se devuelve la cantidad de estas que quedan
	if (FactionsAlive.Contains(Index)) FactionsAlive.Remove(Index);
	return FactionsAlive.Num();
}

void ASMain::RemoveFaction(const int32 Index)
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return;

	// Se elimina la faccion de ambas listas
	if (FactionsAlive.Contains(Index))
	{
		FactionsAlive.Remove(Index);
		Factions.Remove(Index);
	}
}

APawnFaction* ASMain::NextFaction()
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return nullptr;

	// Si no hay una faccion seleccionada, se inicializa el indice de la faccion actual
	if (!CurrentFaction) CurrentIndex = NumFactions - 1;

	// Se invalida la faccion y se realiza la busqueda
	CurrentFaction = nullptr;
	do
	{
		// Se actualiza el indice
		CurrentIndex = (CurrentIndex + 1) % NumFactions;

		// Si se ha dado una vuelta a todas las facciones, se actualiza el numero de turno
		if (CurrentIndex == 0) AddTurn();

		// Si se ha encontrado la faccion, se actualiza y se finaliza
		if (FactionsAlive.Contains(CurrentIndex)) CurrentFaction = Factions[CurrentIndex];
	}
	while (!CurrentFaction);

	return CurrentFaction;
}

//--------------------------------------------------------------------------------------------------------------------//

void ASMain::StartWar(const int32 FactionA, const int32 FactionB)
{
	// Se anade la nueva entrada si no existe, en caso contrario, se restablecen los atributos
	if (!CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
	{
		CurrentWars.Add(FFactionsPair(FactionA, FactionB), FWarInfo());
	}
	else
	{
		CurrentWars[FFactionsPair(FactionA, FactionB)].WarScore = 0.0;
		CurrentWars[FFactionsPair(FactionA, FactionB)].NumTurns = 0;
	}
}

float ASMain::GetWarScore(const int32 FactionA, const int32 FactionB) const
{
	return CurrentWars.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentWars[FFactionsPair(FactionA, FactionB)].WarScore
		       : -1.0;
}

int32 ASMain::GetWarTurns(const int32 FactionA, const int32 FactionB) const
{
	return CurrentWars.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentWars[FFactionsPair(FactionA, FactionB)].NumTurns
		       : -1;
}

void ASMain::EndWar(const int32 FactionA, const int32 FactionB)
{
	// Se verifica que la entrada existe y se elimina
	if (CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
	{
		CurrentWars.Remove(FFactionsPair(FactionA, FactionB));
	}
}

//--------------------------------------------------------------------------------------------------------------------//

int32 ASMain::AddTurn()
{
	return ++CurrentTurn;
}

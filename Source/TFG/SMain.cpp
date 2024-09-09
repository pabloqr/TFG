// Fill out your copyright notice in the Description page of Project Settings.


#include "SMain.h"

TMap<FFactionsPair, FRelationshipInfo> ASMain::GetCurrentWarsForFaction(const int32 Faction) const
{
	TMap<FFactionsPair, FRelationshipInfo> CurrentWarsForFaction = TMap<FFactionsPair, FRelationshipInfo>();

	if (FactionsAlive.Contains(Faction) && Factions.Contains(Faction))
	{
		for (const auto War : CurrentWars)
		{
			if (War.Key.Contains(Faction)) CurrentWarsForFaction.Add(War.Key, War.Value);
		}
	}

	return CurrentWarsForFaction;
}

TMap<FFactionsPair, FRelationshipInfo> ASMain::GetCurrentAlliancesForFaction(const int32 Faction) const
{
	TMap<FFactionsPair, FRelationshipInfo> CurrentAlliancesForFaction = TMap<FFactionsPair, FRelationshipInfo>();

	if (FactionsAlive.Contains(Faction) && Factions.Contains(Faction))
	{
		for (const auto War : CurrentWars)
		{
			if (War.Key.Contains(Faction)) CurrentAlliancesForFaction.Add(War.Key, War.Value);
		}
	}

	return CurrentAlliancesForFaction;
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

void ASMain::ResetFaction(const int32 Index)
{
	// Se eliminan las guerras y alianzas que pudiese tener activas
	for (const auto Faction : FactionsAlive)
	{
		EndWar(Index, Faction);
		EndAlliance(Index, Faction);
	}

	// Se restablecen los datos de la faccion
	if (Factions.Contains(Index)) Factions[Index]->CleanInfoAfterLosing();
}

int32 ASMain::SetFactionDead(const int32 Index)
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return 0;

	// Se verifica que la faccion es valida
	if (FactionsAlive.Contains(Index))
	{
		// Se elimina el indice de la lista de facciones en juego
		FactionsAlive.Remove(Index);

		// Se restablece toda la informacion de la faccion
		ResetFaction(Index);
	}

	// Se devuelve el numero de facciones aun en juego
	return FactionsAlive.Num();
}

void ASMain::RemoveFaction(const int32 Index)
{
	// Se elimina la faccion de ambas listas
	FactionsAlive.Remove(Index);
	Factions.Remove(Index);
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
		if (CurrentIndex == 0)
		{
			AddTurn();

			// Tambien se atualiza el numero de turno de todas las guerras y alianzas activas
			UpdateWarsTurns();
			UpdateAlliancesTurns();
		}

		// Si se ha encontrado la faccion, se actualiza y se finaliza
		if (FactionsAlive.Contains(CurrentIndex)) CurrentFaction = Factions[CurrentIndex];
	}
	while (!CurrentFaction);

	return CurrentFaction;
}

//--------------------------------------------------------------------------------------------------------------------//

void ASMain::UpdateWarsTurns()
{
	// Se procesan todas las guerras
	for (const auto War : CurrentWars)
	{
		++CurrentWars[War.Key].Turns;
		++CurrentWars[War.Key].PetitionTurns;
	}
}

void ASMain::StartWar(const int32 FactionA, const int32 FactionB)
{
	// Se anade la nueva entrada si no existe, en caso contrario, se restablecen los atributos
	if (!CurrentWars.Contains(FFactionsPair(FactionA, FactionB)) &&
		!CurrentWars.Contains(FFactionsPair(FactionB, FactionA)))
	{
		CurrentWars.Add(FFactionsPair(FactionA, FactionB), FRelationshipInfo());
	}
	else
	{
		if (CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
		{
			CurrentWars[FFactionsPair(FactionA, FactionB)].Score = 0.0;
			CurrentWars[FFactionsPair(FactionA, FactionB)].Turns = 0;
		}
		else
		{
			CurrentWars[FFactionsPair(FactionB, FactionA)].Score = 0.0;
			CurrentWars[FFactionsPair(FactionB, FactionA)].Turns = 0;
		}
	}
}

float ASMain::GetWarScore(const int32 FactionA, const int32 FactionB) const
{
	return CurrentWars.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentWars[FFactionsPair(FactionA, FactionB)].Score
		       : CurrentWars.Contains(FFactionsPair(FactionB, FactionA))
		       ? CurrentWars[FFactionsPair(FactionB, FactionA)].Score
		       : 0.0;
}

int32 ASMain::GetWarTurns(const int32 FactionA, const int32 FactionB) const
{
	return CurrentWars.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentWars[FFactionsPair(FactionA, FactionB)].Turns
		       : CurrentWars.Contains(FFactionsPair(FactionB, FactionA))
		       ? CurrentWars[FFactionsPair(FactionB, FactionA)].Turns
		       : -1;
}

void ASMain::UpdateWarScore(const int32 FactionA, const int32 FactionB, const int32 ElementOwner,
                            const float ElementStrength)
{
	// Se verifica que exista la guerra
	if (CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
	{
		// Se calcula la puntuacion de guerra en funcion de la faccion propietaria del elemento destruido
		const float WarScore = FactionA == ElementOwner ? -ElementStrength : ElementStrength;

		CurrentWars[FFactionsPair(FactionA, FactionB)].Score += WarScore;
	}
	else if (CurrentWars.Contains(FFactionsPair(FactionB, FactionA)))
	{
		// Se calcula la puntuacion de guerra en funcion de la faccion propietaria del elemento destruido
		const float WarScore = FactionA == ElementOwner ? ElementStrength : -ElementStrength;

		CurrentWars[FFactionsPair(FactionB, FactionA)].Score += WarScore;
	}
}

void ASMain::ResetWarPetitionTurns(const int32 FactionA, const int32 FactionB)
{
	// Se verifica que exista la guerra
	if (CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
	{
		// Se restablecen los turnos
		CurrentWars[FFactionsPair(FactionA, FactionB)].PetitionTurns = 0;
	}
	else if (CurrentWars.Contains(FFactionsPair(FactionB, FactionA)))
	{
		// Se restablecen los turnos
		CurrentWars[FFactionsPair(FactionB, FactionA)].PetitionTurns = 0;
	}
}

void ASMain::EndWar(const int32 FactionA, const int32 FactionB)
{
	// Se verifica que la entrada existe y se elimina
	if (CurrentWars.Contains(FFactionsPair(FactionA, FactionB)))
	{
		CurrentWars.Remove(FFactionsPair(FactionA, FactionB));
	}
	else if (CurrentWars.Contains(FFactionsPair(FactionB, FactionA)))
	{
		CurrentWars.Remove(FFactionsPair(FactionB, FactionA));
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void ASMain::UpdateAlliancesTurns()
{
	// Se procesan todas las alianzas
	for (const auto Alliance : CurrentAlliances)
	{
		++CurrentAlliances[Alliance.Key].Turns;
	}
}

void ASMain::StartAlliance(const int32 FactionA, const int32 FactionB)
{
	// Se anade la nueva entrada si no existe, en caso contrario, se restablecen los atributos
	if (!CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB)) &&
		!CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA)))
	{
		CurrentAlliances.Add(FFactionsPair(FactionA, FactionB), FRelationshipInfo());
	}
	else
	{
		if (CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB)))
		{
			CurrentAlliances[FFactionsPair(FactionA, FactionB)].Score = 0.0;
			CurrentAlliances[FFactionsPair(FactionA, FactionB)].Turns = 0;
		}
		else
		{
			CurrentAlliances[FFactionsPair(FactionB, FactionA)].Score = 0.0;
			CurrentAlliances[FFactionsPair(FactionB, FactionA)].Turns = 0;
		}
	}
}

float ASMain::GetAllianceScore(const int32 FactionA, const int32 FactionB) const
{
	return CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentAlliances[FFactionsPair(FactionA, FactionB)].Score
		       : CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA))
		       ? CurrentAlliances[FFactionsPair(FactionB, FactionA)].Score
		       : 0.0;
}

int32 ASMain::GetAllianceTurns(const int32 FactionA, const int32 FactionB) const
{
	return CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB))
		       ? CurrentAlliances[FFactionsPair(FactionA, FactionB)].Turns
		       : CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA))
		       ? CurrentAlliances[FFactionsPair(FactionB, FactionA)].Turns
		       : -1;
}

void ASMain::UpdateAllianceScore(const int32 FactionA, const int32 FactionB,
                                 const float AllianceScore)
{
	// Se verifica que exista la alianza
	if (CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB)))
	{
		// Se calcula la puntuacion de la alianza
		CurrentAlliances[FFactionsPair(FactionA, FactionB)].Score += AllianceScore;
	}
	else if (CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA)))
	{
		// Se calcula la puntuacion de la alianza
		CurrentAlliances[FFactionsPair(FactionB, FactionA)].Score -= AllianceScore;
	}
}

void ASMain::ResetAlliancePetitionTurns(const int32 FactionA, const int32 FactionB)
{
	// Se verifica que exista la alianza
	if (CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB)))
	{
		// Se restablecen los turnos
		CurrentAlliances[FFactionsPair(FactionA, FactionB)].PetitionTurns = 0;
	}
	else if (CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA)))
	{
		// Se restablecen los turnos
		CurrentAlliances[FFactionsPair(FactionB, FactionA)].PetitionTurns = 0;
	}
}

void ASMain::EndAlliance(const int32 FactionA, const int32 FactionB)
{
	// Se verifica que la entrada existe y se elimina
	if (CurrentAlliances.Contains(FFactionsPair(FactionA, FactionB)))
	{
		CurrentAlliances.Remove(FFactionsPair(FactionA, FactionB));
	}
	else if (CurrentAlliances.Contains(FFactionsPair(FactionB, FactionA)))
	{
		CurrentAlliances.Remove(FFactionsPair(FactionB, FactionA));
	}
}

//--------------------------------------------------------------------------------------------------------------------//

int32 ASMain::AddTurn()
{
	return ++CurrentTurn;
}

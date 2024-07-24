// Fill out your copyright notice in the Description page of Project Settings.


#include "SMain.h"

void ASMain::AddFaction(const int32 Index)
{
	// Si la faccion ya se encuentra en la lista, se omite
	if (FactionsAlive.Contains(Index)) return;

	// Se comprueba si es mayor que la ultima faccion para insertarlo
	if (FactionsAlive.Num() == 0 || Index > FactionsAlive.Last()) FactionsAlive.Add(Index);
	else
	{
		// En caso contrario, se busca la posicion correcta de la faccion
		for (int32 i = 0; i < FactionsAlive.Num(); ++i)
		{
			// Si no se trata de la posicion correcta, se continua
			if (Index < FactionsAlive[i]) continue;

			// En caso contrario, se inserta en la posicion actual
			FactionsAlive.Insert(Index, i);
		}
	}
}

int32 ASMain::RemoveFaction(const int32 Index)
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return 0;

	// Se elimina la faccion y se devuelve la cantidad de estas que quedan
	if (FactionsAlive.Contains(Index)) FactionsAlive.Remove(Index);
	return FactionsAlive.Num();
}

APawnFaction* ASMain::NextFaction()
{
	// Si el array esta vacio, no se hace nada
	if (FactionsAlive.Num() == 0) return nullptr;

	// Se obtiene la posicion de la faccion actual
	int32 Pos = FactionsAlive.Find(CurrentFaction);
	if (Pos != INDEX_NONE)
	{
		// Se obtiene la siguiente posicion y se actualiza el indice de la faccion
		Pos = (Pos + 1) % FactionsAlive.Num();
		CurrentFaction = FactionsAlive[Pos];
	}
	else
	{
		// Si no se ha podido encontrar la faccion, se busca la siguiente faccion
		for (int32 i = 0; i < FactionsAlive.Num(); ++i)
		{
			// Si el valor de la faccion es mayor, se trata de la correcta
			if (FactionsAlive[i] > CurrentFaction)
			{
				// Se actualiza el indice y se finaliza el bucle
				CurrentFaction = FactionsAlive[i];
				break;
			}
		}
	}

	return Factions[CurrentFaction];
}

int32 ASMain::AddTurn()
{
	return ++CurrentTurn;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnFaction.h"
#include "GameFramework/GameStateBase.h"
#include "SMain.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API ASMain : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 NumFactions;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 CurrentFaction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TArray<APawnFaction*> Factions = TArray<APawnFaction*>();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TArray<int32> FactionsAlive = TArray<int32>();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 CurrentTurn;

public:
	int32 GetNumFactions() const { return NumFactions; }
	int32 GetCurrentFaction() const { return CurrentFaction; }
	const TArray<int32>& GetFactionsAlive() const { return FactionsAlive; }
	int32 GetCurrentTurn() const { return CurrentTurn; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetNumFactions(const int32 NFactions) { NumFactions = NFactions; }
	void SetCurrentFaction(const int32 Faction) { CurrentFaction = Faction; }
	void SetFactionsAlive(const TArray<int32>& FAlive) { FactionsAlive = FAlive; }
	void SetCurrentTurn(const int32 Turn) { CurrentTurn = Turn; }

	//----------------------------------------------------------------------------------------------------------------//

	void AddFaction(const int32 Index);
	int32 RemoveFaction(const int32 Index);

	/**
	 * Metodo que actualiza y devuelve el indice de la faccion que debe jugar su turno
	 * 
	 * @return Numero de la faccion actual
	 */
	APawnFaction* NextFaction();

	/**
	 * Metodo que actualiza y devuelve el numero del turno que se va a comenzar
	 * 
	 * @return Numero del turno actual
	 */
	int32 AddTurn();
};

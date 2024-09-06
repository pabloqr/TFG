// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFactionsPair.h"
#include "FRelationshipInfo.h"
#include "PawnFaction.h"
#include "GameFramework/GameStateBase.h"
#include "SMain.generated.h"

//--------------------------------------------------------------------------------------------------------------------//

/**
 * 
 */
UCLASS()
class TFG_API ASMain : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	FString CurrentGameName;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Numero de facciones en juego al inicio de la partida
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 NumFactions;

	/**
	 * Indice de la faccion en juego
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 CurrentIndex;
	/**
	 * Faccion en juego
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	APawnFaction* CurrentFaction;

	/**
	 * Diccionario de facciones clasificadas por su indice
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TMap<int32, APawnFaction*> Factions = TMap<int32, APawnFaction*>();

	/**
	 * Contenedor de facciones aun en juego
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TSet<int32> FactionsAlive = TSet<int32>();

	/**
	 * Diccionario de guerras en curso en la partida
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TMap<FFactionsPair, FRelationshipInfo> CurrentWars = TMap<FFactionsPair, FRelationshipInfo>();

	/**
	 * Diccionario de guerras en curso en la partida
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	TMap<FFactionsPair, FRelationshipInfo> CurrentAlliances = TMap<FFactionsPair, FRelationshipInfo>();

	/**
	 * Numero del turno actual
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MainState")
	int32 CurrentTurn;

public:
	/**
	 * Getter del atributo NumFactions
	 * 
	 * @return Numero de facciones creadas al inicio de la partida
	 */
	int32 GetNumFactions() const { return NumFactions; }

	/**
	 * Getter del atributo CurrentIndex
	 * 
	 * @return Indice de la faccion en juego actualmente
	 */
	int32 GetCurrentIndex() const { return CurrentIndex; }

	/**
	 * Getter del atributo CurrentFaction
	 * 
	 * @return Faccion en juego
	 */
	APawnFaction* GetCurrentFaction() const { return CurrentFaction; }

	/**
	 * Getter del atributo Factions
	 * 
	 * @return Diccionario de facciones creadas
	 */
	const TMap<int32, APawnFaction*>& GetFactions() const { return Factions; }

	/**
	 * Getter del atributo FactionsAlive
	 * 
	 * @return Indices de las facciones en juego
	 */
	const TSet<int32>& GetFactionsAlive() const { return FactionsAlive; }

	const TMap<FFactionsPair, FRelationshipInfo>& GetCurrentWars() const { return CurrentWars; }
	TMap<FFactionsPair, FRelationshipInfo> GetCurrentWarsForFaction(const int32 Faction) const;
	const TMap<FFactionsPair, FRelationshipInfo>& GetCurrentAlliances() const { return CurrentAlliances; }
	TMap<FFactionsPair, FRelationshipInfo> GetCurrentAlliancesForFaction(const int32 Faction) const;

	/**
	 * Getter del atributo CurrentTurn
	 * 
	 * @return Numero del turno actual
	 */
	int32 GetCurrentTurn() const { return CurrentTurn; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter del atributo NumFactions
	 * 
	 * @param NFactions Numero de facciones creadas
	 */
	void SetNumFactions(const int32 NFactions) { NumFactions = NFactions; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que anade una faccion a la coleccion de facciones
	 * 
	 * @param Faction Faccion a anadir
	 */
	UFUNCTION(BlueprintCallable)
	void AddFaction(APawnFaction* Faction);

	/**
	 * Metodo que actualiza la coleccion de facciones para reflejar la eliminacion de una de ellas en la partida
	 * 
	 * @param Index Indice de la faccion eliminada
	 * @return Numero de facciones aun en juego
	 */
	UFUNCTION(BlueprintCallable)
	int32 SetFactionDead(const int32 Index);

	/**
	 * Metodo que elimina una faccion de las colecciones
	 * 
	 * @param Index Indice de la faccion a eliminar
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveFaction(const int32 Index);

	/**
	 * Metodo que actualiza y devuelve el indice de la faccion que debe jugar su turno
	 * 
	 * @return Numero de la faccion actual
	 */
	APawnFaction* NextFaction();

	//----------------------------------------------------------------------------------------------------------------//

	void UpdateWarsTurns();

	void StartWar(const int32 FactionA, const int32 FactionB);
	float GetWarScore(const int32 FactionA, const int32 FactionB) const;
	int32 GetWarTurns(const int32 FactionA, const int32 FactionB) const;
	void UpdateWarScore(const int32 FactionA, const int32 FactionB, int32 ElementOwner, const float ElementStrength);
	void ResetWarPetitionTurns(const int32 FactionA, const int32 FactionB);
	void EndWar(const int32 FactionA, const int32 FactionB);

	//----------------------------------------------------------------------------------------------------------------//

	void UpdateAlliancesTurns();

	void StartAlliance(const int32 FactionA, const int32 FactionB);
	float GetAllianceScore(const int32 FactionA, const int32 FactionB) const;
	int32 GetAllianceTurns(const int32 FactionA, const int32 FactionB) const;
	void UpdateAllianceScore(const int32 FactionA, const int32 FactionB, const float AllianceScore);
	void ResetAlliancePetitionTurns(const int32 FactionA, const int32 FactionB);
	void EndAlliance(const int32 FactionA, const int32 FactionB);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que actualiza y devuelve el numero del turno que se va a comenzar
	 * 
	 * @return Numero del turno actual
	 */
	int32 AddTurn();
};

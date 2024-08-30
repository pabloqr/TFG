// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "FFactionInfo.h"
#include "FResourceInfo.h"
#include "FWarInfo.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "PawnFaction.generated.h"

class AActorSettlement;
enum class ESettlementState : uint8;
class AActorDamageableElement;

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyUpdated, float, Money);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyBalanceUpdated, float, MoneyBalance);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API APawnFaction : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	FFactionInfo Info;

public:
	/**
	 * Constructor por defecto
	 */
	APawnFaction();

private:
	void UpdateFactionDiplomaticRelationship(int32 Faction, const EDiplomaticRelationship Relationship);

protected:
	UFUNCTION(BlueprintCallable)
	void OnUnitStateUpdated(const AActorUnit* Unit, const EUnitState State);

	UFUNCTION(BlueprintCallable)
	void OnSettlementStateUpdated(const AActorSettlement* Settlement, const ESettlementState State);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddKnowFaction(const int32 Faction, const FOpponentFactionInfo& FactionInfo);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	int32 GetIndex() const { return Info.Index; }
	float GetMilitaryStrength() const { return Info.MilitaryStrength; }
	float GetMoney() const { return Info.Money; }
	float GetMoneyBalance() const { return Info.MoneyBalance; }
	const TArray<AActorSettlement*>& GetSettlements() const { return Info.Settlements; }
	int32 GetNumSettlements() const { return Info.Settlements.Num(); }
	const TArray<int32>& GetIdleSettlements() const { return Info.IdleSettlements; }
	const TMap<EResource, FResourceCollection>& GetMonetaryResources() const { return Info.MonetaryResources; }
	const TMap<EResource, FResourceCollection>& GetStrategicResources() const { return Info.StrategicResources; }
	const TArray<AActorUnit*>& GetUnits() const { return Info.Units; }
	const TArray<int32>& GetManualUnits() const { return Info.ManualUnits; }
	const TArray<int32>& GetAutomaticUnits() const { return Info.AutomaticUnits; }

	const TMap<int32, FOpponentFactionInfo>& GetKnownFactions() const { return Info.KnownFactions; }

	const TSet<int32>& GetFactionsAtWar() const
	{
		return Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::AtWar].Factions;
	}

	const TSet<int32>& GetNeutralFactions() const
	{
		return Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Neutral].Factions;
	}

	const TSet<int32>& GetAllyFactions() const
	{
		return Info.FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Ally].Factions;
	}

	//----------------------------------------------------------------------------------------------------------------//

	void SetIndex(const int32 I) { Info.Index = I; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	bool CanProduceUnit(const UDataTable* DataTable, const EUnitType UnitType) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddMoney(const int32 Amount);

	UFUNCTION(BlueprintCallable)
	void RemoveMoney(const int32 Amount);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddSettlement(AActorSettlement* Settlement);

	UFUNCTION(BlueprintCallable)
	void RemoveSettlement(AActorSettlement* Settlement);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddUnit(AActorUnit* Unit);

	UFUNCTION(BlueprintCallable)
	void RemoveUnit(AActorUnit* Unit);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void OwnResource(const EResource Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void DisownResource(const EResource Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void AddResource(bool FromDeal, const FResource& Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void RemoveResource(bool FromDeal, const FResource& Resource, const FIntPoint& Pos);

	//----------------------------------------------------------------------------------------------------------------//

	void UpdateKnownFactionsInfo(const TMap<int32, float>& FactionsStrength, TMap<int32, FWarInfo> CurrentWars);

	UFUNCTION(BlueprintCallable)
	void DeclareWarOnFaction(const int32 Faction);

	UFUNCTION(BlueprintCallable)
	void MakePeaceWithFaction(const int32 Faction);

	UFUNCTION(BlueprintCallable)
	void MakeAllianceWithFaction(const int32 Faction);

	UFUNCTION(BlueprintCallable)
	void BreakAllianceWithFaction(const int32 Faction);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnEnded();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que asocia un metodo de entrada para la interaccion entre el jugador y el juego
	 * 
	 * @param PlayerInputComponent Entrada del jugador
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnTurnStarted OnTurnStarted;

	UPROPERTY(BlueprintAssignable)
	FOnMoneyUpdated OnMoneyUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnMoneyBalanceUpdated OnMoneyBalanceUpdated;
};

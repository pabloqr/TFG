// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "FResourceInfo.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "PawnFaction.generated.h"

class AActorSettlement;
enum class ESettlementState : uint8;
class AActorDamageableElement;

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FFactionsSet
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="FactionsArray")
	TSet<int32> Factions;

	FFactionsSet(): FFactionsSet(TSet<int32>())
	{
	}

	explicit FFactionsSet(const TSet<int32>& Factions)
		: Factions(Factions)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FResourceCollection
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	FResource GatheredResource;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	TArray<FIntPoint> Tiles;

	FResourceCollection(): FResourceCollection(FResource(), TArray<FIntPoint>())
	{
	}

	FResourceCollection(const FResource& GatheredResource, const TArray<FIntPoint>& Tiles)
		: GatheredResource(GatheredResource),
		  Tiles(Tiles)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

UENUM(BlueprintType)
enum class EDiplomaticRelationship : uint8
{
	AtWar = 0 UMETA(DisplayName = "AtWar"),
	Neutral = 1 UMETA(DisplayName = "Neutral"),
	Ally = 2 UMETA(DisplayName = "Ally"),
};

USTRUCT(BlueprintType)
struct FOpponentFactionInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="OpponentFactionInfo")
	EDiplomaticRelationship Relationship;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="OpponentFactionInfo")
	float MilitaryStrength;

	FOpponentFactionInfo(): FOpponentFactionInfo(EDiplomaticRelationship::Neutral, 0.0)
	{
	}

	FOpponentFactionInfo(const EDiplomaticRelationship Relationship, const float MilitaryStrength)
		: Relationship(Relationship),
		  MilitaryStrength(MilitaryStrength)
	{
	}
};

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
	int32 Index;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float MilitaryStrength;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float Money;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float MoneyBalance;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorSettlement*> Settlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> IdleSettlements;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorUnit*> Units;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> ManualUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> AutomaticUnits;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> MonetaryResources;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> StrategicResources;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Opponents")
	TMap<int32, FOpponentFactionInfo> KnownFactions;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Opponents")
	TMap<EDiplomaticRelationship, FFactionsSet> FactionsWithDiplomaticRelationship;

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

	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	int32 GetIndex() const { return Index; }
	float GetMilitaryStrength() const { return MilitaryStrength; }
	float GetMoney() const { return Money; }
	float GetMoneyBalance() const { return MoneyBalance; }
	const TArray<AActorSettlement*>& GetSettlements() const { return Settlements; }
	int32 GetNumSettlements() const { return Settlements.Num(); }
	const TArray<int32>& GetIdleSettlements() const { return IdleSettlements; }
	const TMap<EResource, FResourceCollection>& GetMonetaryResources() const { return MonetaryResources; }
	const TMap<EResource, FResourceCollection>& GetStrategicResources() const { return StrategicResources; }
	const TArray<AActorUnit*>& GetUnits() const { return Units; }
	const TArray<int32>& GetManualUnits() const { return ManualUnits; }
	const TArray<int32>& GetAutomaticUnits() const { return AutomaticUnits; }

	const TSet<int32>& GetFactionsAtWar() const
	{
		return FactionsWithDiplomaticRelationship[EDiplomaticRelationship::AtWar].Factions;
	}

	const TSet<int32>& GetNeutralFactions() const
	{
		return FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Neutral].Factions;
	}

	const TSet<int32>& GetAllyFactions() const
	{
		return FactionsWithDiplomaticRelationship[EDiplomaticRelationship::Ally].Factions;
	}

	//----------------------------------------------------------------------------------------------------------------//

	void SetIndex(const int32 I) { Index = I; }

	//----------------------------------------------------------------------------------------------------------------//

	bool HasElement(const AActorDamageableElement* Element) const;

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

	void UpdateKnownFactionsInfo(const TMap<int32, float>& FactionsStrength);

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

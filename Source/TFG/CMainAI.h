// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorUnit.h"
#include "InterfaceDeal.h"
#include "MMain.h"
#include "TPriorityQueue.h"
#include "CMainAI.generated.h"

class UDataTable;
class AActorSettlement;
class AActorTileMap;
class APawnFaction;

UENUM(BlueprintType)
enum class EMilitaryState : uint8
{
	Defensive = 0 UMETA(DisplayName = "Defensive"),
	Neutral = 1 UMETA(DisplayName = "Neutral"),
	Offensive = 2 UMETA(DisplayName = "Offensive"),
};

UENUM(BlueprintType)
enum class EUnitAction : uint8
{
	None = 0,
	MoveAround = 1,
	MoveTowardsEnemy = 2,
	MoveAwayFromEnemy = 3,
	MoveTowardsAlly = 4,
	MoveTowardsEnemyTiles = 5,
	MoveTowardsAllyTiles = 6,
	Heal = 7
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FTileValue
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="TileValue")
	FIntPoint Pos;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="TileValue")
	float Value;

	FTileValue(): FTileValue(-1, 0)
	{
	}

	FTileValue(const FIntPoint& Pos, const float Value)
		: Pos(Pos),
		  Value(Value)
	{
	}
	
	/**
	 * Operador <
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor que el que se compara
	 */
	bool operator<(const FTileValue& Other) const
	{
		return Value > Other.Value;
	}

	/**
	 * Operador >
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor que el que se compara
	 */
	bool operator>(const FTileValue& Other) const
	{
		return !(*this < Other || *this == Other);
	}

	/**
	 * Operador ==
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es igual que el que se compara
	 */
	bool operator==(const FTileValue& Other) const
	{
		return Value == Other.Value;
	}

	/**
	 * Operador !=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es diferente que el que se compara
	 */
	bool operator!=(const FTileValue& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * Operador <=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor o igual que el que se compara
	 */
	bool operator<=(const FTileValue& Other) const
	{
		return *this < Other || *this == Other;
	}

	/**
	 * Operador >=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor o igual que el que se compara
	 */
	bool operator>=(const FTileValue& Other) const
	{
		return *this > Other || *this == Other;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitProductionSelection, AActorSettlement*, Settlement,
                                             EUnitType, UnitType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnFinished);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API ACMainAI : public AAIController, public IInterfaceDeal
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	APawnFaction* PawnFaction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	AActorTileMap* TileMap;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	UDataTable* UnitDataTable;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	EMilitaryState MilitaryState;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	TArray<int32> FactionsIndex;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Cola con prioridad que almacena la mejor casilla y el valor de atractivo para establecer un asentamiento
	 * en cada turno
	 */
	TPriorityQueue<FTileValue> BestTileForSettlement;

	/**
	 * Array de posiciones en las que se ha planificado establecer un asentamiento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="AI")
	TArray<FIntPoint> PlannedSettlements;

	/**
	 * Diccionario que almacena los valores de atractivo de las casillas
	 */
	TMap<FIntPoint, float> TilesValue;

	//----------------------------------------------------------------------------------------------------------------//

	TSet<FIntPoint> EnemiesLocation;

	TSet<FIntPoint> AlliesLocation;

	//----------------------------------------------------------------------------------------------------------------//

	int32 UnitsMoving;

public:
	/**
	 * Constructor por defecto que inicializa los atributos de la clase
	 */
	ACMainAI();

private:
	static float CalculateStrengthDifferenceRelevance(const float StrengthA, const float StrengthB);

	//----------------------------------------------------------------------------------------------------------------//

	static const AActorSettlement* GetClosestSettlementFromPos(const FIntPoint& Pos,
	                                                           const TArray<AActorSettlement*>& Settlements);
	FIntPoint GetClosestTilePos(const FIntPoint& Pos, TArray<FIntPoint>& SettlementOwnedTiles) const;

	//----------------------------------------------------------------------------------------------------------------//

	void UpdateTilesValue();

	bool IsSettlementNeeded() const;
	bool IsResourceGatheringNeeded() const;
	FIntPoint CalculateBestPosForSettlement();

	FIntPoint GetClosestResourceToGatherPos(const FIntPoint& Pos) const;

	//----------------------------------------------------------------------------------------------------------------//

	TSet<FIntPoint> GetEnemyOrAllyLocationInRange(const FIntPoint& Pos, const int32 Range, const bool GetEnemy) const;

	const AActorSettlement* GetClosestEnemySettlementFromPos(const FIntPoint& Pos) const;
	const AActorSettlement* GetClosestOwnedSettlementFromPos(const FIntPoint& Pos) const;

	FIntPoint GetFarthestPosFromEnemies(const FIntPoint& Pos, const int32 Range) const;
	FIntPoint GetClosestPosToAlly(const FIntPoint& Pos) const;
	FIntPoint GetClosestEnemyTilePos(const FIntPoint& Pos) const;
	FIntPoint GetClosestAllyTilePos(const FIntPoint& Pos) const;

	FIntPoint CalculateBestPosForUnit(const FUnitInfo& UnitInfo, const EUnitAction UnitAction) const;

	//----------------------------------------------------------------------------------------------------------------//

	EUnitType CalculateBestUnitTypeToProduce() const;

	//----------------------------------------------------------------------------------------------------------------//

	void ManageFactionAtWar(const int32 FactionIndex);
	void ManageNeutralFaction(const int32 FactionIndex);
	void ManageAllyFaction(const int32 FactionIndex);

	void ManageCivilUnit(AActorUnit* Unit);
	void ManageMilitaryUnit(AActorUnit* Unit) const;

	//----------------------------------------------------------------------------------------------------------------//

	void ManageDiplomacy();
	void ManageElements();
	void ManageUnits();
	void ManageSettlementsProduction() const;

protected:
	UFUNCTION(BlueprintCallable)
	bool CanFinishTurn() const;

	//----------------------------------------------------------------------------------------------------------------//

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void ManageNextFactionAtWar();
	void ManageNextNeutralFaction();
	void ManageNextAllyFaction();

	UFUNCTION(BlueprintCallable)
	virtual void ProposeDeal(const FDealInfo& Deal) const override;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnFinished();

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnUnitProductionSelection OnUnitProductionSelection;

	UPROPERTY(BlueprintAssignable)
	FOnTurnFinished OnTurnFinished;
};

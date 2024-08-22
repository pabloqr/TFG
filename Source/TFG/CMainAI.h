// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorUnit.h"
#include "MMain.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitProductionSelection, AActorSettlement*, Settlement,
                                             EUnitType, UnitType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnFinished);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API ACMainAI : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	APawnFaction* PawnFaction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	AActorTileMap* TileMap;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="AI")
	EMilitaryState MilitaryState;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Atributo que almacena la mejor casilla y el valor de atractivo para establecer un asentamiento en cada turno
	 */
	TTuple<FIntPoint, float> BestTileForSettlement;

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

	void ManageCivilUnit(AActorUnit* Unit);
	void ManageMilitaryUnit(AActorUnit* Unit) const;

	//----------------------------------------------------------------------------------------------------------------//

	EUnitType CalculateBestUnitTypeToProduce() const;

	//----------------------------------------------------------------------------------------------------------------//

	float CalculateMoneyAmountForPeaceTreaty(const bool ImLoosing, const float WarScore,
	                                         float StrengthDiffRel) const;

	//----------------------------------------------------------------------------------------------------------------//

	void ManageDiplomacy() const;
	void ManageUnits();
	void ManageSettlementsProduction() const;

protected:
	UFUNCTION(BlueprintCallable)
	bool CanFinishTurn() const;

	//----------------------------------------------------------------------------------------------------------------//

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	float ProposePeaceTreaty(const FDealInfo& Deal) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnFinished() const;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnUnitProductionSelection OnUnitProductionSelection;

	UPROPERTY(BlueprintAssignable)
	FOnTurnFinished OnTurnFinished;
};

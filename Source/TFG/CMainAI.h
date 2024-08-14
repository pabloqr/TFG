// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActorUnit.h"
#include "CMainAI.generated.h"

class AActorSettlement;
class AActorTileMap;
class APawnFaction;

UENUM()
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

	TSet<FIntPoint> EnemiesLocation;

	TSet<FIntPoint> AlliesLocation;

public:
	/**
	 * Constructor por defecto que inicializa los atributos de la clase
	 */
	ACMainAI();

private:
	static const AActorSettlement* GetClosestSettlementFromPos(const FIntPoint& Pos,
	                                                           const TArray<AActorSettlement*>& Settlements);
	FIntPoint GetClosestTilePos(const FIntPoint& Pos, TArray<FIntPoint>& SettlementOwnedTiles) const;

	//----------------------------------------------------------------------------------------------------------------//

	bool IsSettlementNeeded() const;
	FIntPoint CalculateBestPosForSettlement() const;

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

	void ManageCivilUnit(AActorUnit* Unit) const;
	void ManageMilitaryUnit(AActorUnit* Unit) const;

	void ManageUnits();
	void ManageSettlementsProduction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnFinished();
};

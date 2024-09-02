// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FProductionElement.h"
#include "FSettlementInfo.generated.h"

UENUM(BlueprintType)
enum class ESettlementState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	SelectProduction = 1 UMETA(DisplayName="SelectProduction"),
	Producing = 2 UMETA(DisplayName="Producing"),
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FSettlementInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	FIntPoint Pos2D;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	int32 TurnsToOwnTile;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FIntPoint> OwnedTiles;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FProductionElement> ProductionQueue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FProductionElement> StartedProduction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	float MoneyYield;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	ESettlementState State;

	FSettlementInfo(): FSettlementInfo(FIntPoint(-1), 0, TArray<FIntPoint>(),
	                                   TArray<FProductionElement>(), TArray<FProductionElement>(),
	                                   0.0, ESettlementState::None)
	{
	}

	FSettlementInfo(const FIntPoint& Pos2D, const int32 TurnsToOwnTile, const TArray<FIntPoint>& OwnedTiles,
	                const TArray<FProductionElement>& ProductionQueue,
	                const TArray<FProductionElement>& StartedProduction,
	                const float MoneyYield, const ESettlementState State)
		: Pos2D(Pos2D),
		  TurnsToOwnTile(TurnsToOwnTile),
		  OwnedTiles(OwnedTiles),
		  ProductionQueue(ProductionQueue),
		  StartedProduction(StartedProduction),
		  MoneyYield(MoneyYield),
		  State(State)
	{
	}
};

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
	TArray<FIntPoint> OwnedTiles;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FProductionElement> ProductionQueue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	float MoneyYield;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	ESettlementState State;

	FSettlementInfo(): FSettlementInfo(FIntPoint(-1), TArray<FIntPoint>(), TArray<FProductionElement>(),
	                                   0.0, ESettlementState::None)
	{
	}

	FSettlementInfo(const FIntPoint& Pos, const TArray<FIntPoint>& Tiles, const TArray<FProductionElement>& Queue,
	                const float Yield, const ESettlementState SettlementState)
	{
		Pos2D = Pos;
		OwnedTiles = Tiles;
		ProductionQueue = Queue;
		MoneyYield = Yield;
		State = SettlementState;
	}
};

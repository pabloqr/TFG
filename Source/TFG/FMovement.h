// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMovement.generated.h"

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	int32 MovementCost;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	int32 TotalCost;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	int32 TotalTurns;

	FMovement() : FMovement(FIntPoint(-1, -1), -1, -1)
	{
	}

	FMovement(const FIntPoint& Pos, const int32 MCost, const int32 TCost, const int32 TTurns = -1)
	{
		Pos2D = Pos;
		MovementCost = MCost;
		TotalCost = TCost;
		TotalTurns = TTurns;
	}
};

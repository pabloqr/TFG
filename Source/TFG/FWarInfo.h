// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FWarInfo.generated.h"

USTRUCT(BlueprintType)
struct FWarInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="WarInfo")
	float WarScore;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="WarInfo")
	int32 NumTurns;

	FWarInfo(): FWarInfo(0.0, 0)
	{
	}

	FWarInfo(const float WarScore, const int32 NumTurns)
		: WarScore(WarScore),
		  NumTurns(NumTurns)
	{
	}
};

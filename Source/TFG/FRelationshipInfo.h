// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FRelationshipInfo.generated.h"

USTRUCT(BlueprintType)
struct FRelationshipInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="RelationshipInfo")
	float Score;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="RelationshipInfo")
	int32 Turns;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="RelationshipInfo")
	int32 PetitionTurns;

	FRelationshipInfo(): FRelationshipInfo(0.0, 0, 0)
	{
	}

	FRelationshipInfo(const float Score, const int32 Turns, const int32 PetitionTurns)
		: Score(Score),
		  Turns(Turns),
		  PetitionTurns(PetitionTurns)
	{
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFactionsPair.generated.h"

USTRUCT(BlueprintType)
struct FFactionsPair
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="FactionsPair")
	int32 FactionA;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="FactionsPair")
	int32 FactionB;

	//----------------------------------------------------------------------------------------------------------------//

	FFactionsPair(): FFactionsPair(-1, -1)
	{
	}

	FFactionsPair(const int32 FactionA, const int32 FactionB)
		: FactionA(FactionA),
		  FactionB(FactionB)
	{
	}

	//----------------------------------------------------------------------------------------------------------------//

	bool Contains(const int32 Faction) const
	{
		return FactionA == Faction || FactionB == Faction;
	}

	//----------------------------------------------------------------------------------------------------------------//

	bool operator==(const FFactionsPair& Other) const
	{
		return FactionA == Other.FactionA && FactionB == Other.FactionB;
	}

	friend uint32 GetTypeHash(const FFactionsPair& Other)
	{
		return GetTypeHash(Other.FactionA) + GetTypeHash(Other.FactionB);
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FCivilUnitInfo.generated.h"

/**
 * Tipo enumerado para determinar el estado de una unidad civil
 */
UENUM(BlueprintType)
enum class ECivilUnitState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	WaitingForNextTurn = 1 UMETA(DisplayName"WaitingForNextTurn"),
	GatheringResource = 2 UMETA(DisplayName="GatheringResource"),
	SettingSettlement = 3 UMETA(DisplayName="SettingSettlement"),
};

USTRUCT(BlueprintType)
struct FCivilUnitInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Civil")
	int32 NumActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Civil")
	FIntPoint TargetPos;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Civil")
	ECivilUnitState State;

	FCivilUnitInfo(): FCivilUnitInfo(0, -1, ECivilUnitState::None)
	{
	}

	FCivilUnitInfo(const int32 NumActions, const FIntPoint& TargetPos, const ECivilUnitState State)
		: NumActions(NumActions),
		  TargetPos(TargetPos),
		  State(State)
	{
	}
};

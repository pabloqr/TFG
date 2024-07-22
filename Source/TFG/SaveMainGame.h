// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveMap.h"
#include "GameFramework/SaveGame.h"
#include "SaveMainGame.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API USaveMainGame : public USaveMap
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	FString MapSaveName;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	TArray<FUnitSaveData> Units;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	TArray<FSettlementSaveData> Settlements;
};

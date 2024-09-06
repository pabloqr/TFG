// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFactionsPair.h"
#include "FRelationshipInfo.h"
#include "SaveMap.h"
#include "GameFramework/SaveGame.h"
#include "SaveMainGame.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API USaveMainGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|General")
	FString MapSaveName;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	int NumFactions;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	int32 CurrentIndex;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	TSet<int32> FactionsAlive;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	TMap<FFactionsPair, FRelationshipInfo> CurrentWars;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	TMap<FFactionsPair, FRelationshipInfo> CurrentAlliances;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|State")
	int32 CurrentTurn;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|Elements")
	TArray<FUnitSaveData> Units;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Game|Elements")
	TArray<FSettlementSaveData> Settlements;
};

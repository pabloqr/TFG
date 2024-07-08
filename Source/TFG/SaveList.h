// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSaveStructures.h"
#include "GameFramework/SaveGame.h"
#include "SaveList.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API USaveList : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|List")
	TArray<FSaveData> MapSaves;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|List")
	TArray<FSaveData> GameSaves;
};

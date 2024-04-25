// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonManager.h"
#include "GameFramework/SaveGame.h"
#include "SaveMap.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API USaveMap : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FMapData> TilesInfo;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSaveStructures.h"
#include "GameFramework/SaveGame.h"
#include "SaveMap.generated.h"

/**
 * Tipo enumerado para la clasificacion de mapas segun su temperatura
 */
UENUM(BlueprintType)
enum class EMapTemperature : uint8
{
	Desert = 0 UMETA(DisplayName="Desert"),
	Hot = 1 UMETA(DisplayName="Hot"),
	Temperate = 2 UMETA(DisplayName="Temperate"),
	Cold = 3 UMETA(DisplayName="Cold"),
};

/**
 * Tipo enumerado para la clasificacion de mapas segun su nivel del mar (cantidad de casillas de agua)
 */
UENUM(BlueprintType)
enum class EMapSeaLevel : uint8
{
	Arid = 0 UMETA(DisplayName="Arid"),
	Standard = 1 UMETA(DisplayName="Standard"),
	Wet = 2 UMETA(DisplayName="Wet"),
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * 
 */
UCLASS()
class TFG_API USaveMap : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	EMapTemperature MapTemperature;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	EMapSeaLevel MapSeaLevel;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	float WaterTileChance;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	TArray<FTileSaveData> Tiles;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|Map")
	TArray<FResourceInfo> Resources;
};

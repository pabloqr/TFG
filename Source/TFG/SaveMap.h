// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorTileMap.h"
#include "FSettlementInfo.h"
#include "FUnitInfo.h"
#include "GameFramework/SaveGame.h"
#include "SaveMap.generated.h"

/**
 * Estructura que almacena la informacion sobre las casillas a guardar en un archivo de texto
 */
USTRUCT(BlueprintType, Category="Saves")
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|MapData")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|MapData")
	ETileType Type;

	FMapData(): FMapData(FIntPoint(-1), ETileType::None) {}

	FMapData(const FIntPoint& Pos, const ETileType TileType)
	{
		Pos2D = Pos;
		Type = TileType;
	}
};

USTRUCT(BlueprintType)
struct FUnitData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|UnitData")
	int32 Owner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|UnitData")
	FUnitInfo Info;
};

USTRUCT(BlueprintType)
struct FSettlementData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|UnitData")
	int32 Owner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves|UnitData")
	FSettlementInfo Info;
};

/**
 * 
 */
UCLASS()
class TFG_API USaveMap : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	TArray<FMapData> Tiles;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	TArray<FUnitData> Units;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	TArray<FSettlementData> Settlements;
};

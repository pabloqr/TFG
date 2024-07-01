// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSettlementInfo.h"
#include "FTileInfo.h"
#include "FUnitInfo.h"
#include "FSaveStructures.generated.h"

/**
 * Estructura que almacena la informacion sobre las casillas a guardar en un archivo de texto
 */
USTRUCT(BlueprintType)
struct FMapSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	ETileType Type;

	FMapSaveData(): FMapSaveData(FIntPoint(-1), ETileType::None)
	{
	}

	FMapSaveData(const FIntPoint& Pos, const ETileType TileType)
	{
		Pos2D = Pos;
		Type = TileType;
	}
};

USTRUCT(BlueprintType)
struct FUnitSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	int32 Owner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FUnitInfo Info;

	FUnitSaveData(): FUnitSaveData(-1, FUnitInfo())
	{
	}

	FUnitSaveData(const int32 Owner, const FUnitInfo& Info)
		: Owner(Owner),
		  Info(Info)
	{
	}
};

USTRUCT(BlueprintType)
struct FSettlementSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	int32 Owner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FSettlementInfo Info;
};

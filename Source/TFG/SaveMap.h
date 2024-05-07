// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveMap.generated.h"

/**
 * Estructura que almacena la informacion sobre las casillas a guardar en un archivo de texto
 */
USTRUCT(BlueprintType, Category="Saves|Map")
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Row;
	UPROPERTY()
	int32 Col;
	UPROPERTY()
	int32 TileType;

	FMapData()
	{
		Row = -1;
		Col = -1;
		TileType = -1;
	}

	FMapData(const int32 Row, const int32 Col, const int32 TileType)
	{
		this->Row = Row;
		this->Col = Col;
		this->TileType = TileType;
	}
};

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

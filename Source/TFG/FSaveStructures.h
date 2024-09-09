// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSettlementInfo.h"
#include "FTileInfo.h"
#include "FUnitInfo.h"
#include "FSaveStructures.generated.h"

UENUM(BlueprintType)
enum class ESaveType : uint8
{
	MapSave = 0 UMETA(DisplayName="MapSave"),
	GameSave = 1 UMETA(DisplayName="GameSave")
};

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FString SaveName;
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FDateTime SaveDate;
	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FString CustomName;

	FSaveData(): FSaveData(TEXT(""), FDateTime::Now(), TEXT(""))
	{
	}

	FSaveData(const FString& SaveName, const FDateTime& SaveDate, const FString& CustomName)
		: SaveName(SaveName),
		  SaveDate(SaveDate),
		  CustomName(CustomName)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Estructura que almacena la informacion sobre las casillas para un archivo de guardado
 */
USTRUCT(BlueprintType)
struct FTileSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FIntPoint Pos2D;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	int32 Owner;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	ETileType Type;

	FTileSaveData(): FTileSaveData(FIntPoint(-1), -1, ETileType::None)
	{
	}

	FTileSaveData(const FIntPoint& Pos2D, const int32 Owner, const ETileType Type)
		: Pos2D(Pos2D),
		  Owner(Owner),
		  Type(Type)
	{
	}
};

/**
 * Estructura que almacena la informacion sobre las unidades para un archivo de guardado
 */
USTRUCT(BlueprintType)
struct FUnitSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	int32 Owner;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	float HealthPoints;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FUnitInfo Info;

	FUnitSaveData(): FUnitSaveData(-1, 0.0, FUnitInfo())
	{
	}

	FUnitSaveData(const int32 Owner, const float HealthPoints, const FUnitInfo& Info)
		: Owner(Owner),
		  HealthPoints(HealthPoints),
		  Info(Info)
	{
	}
};

/**
 * Estructura que almacena la informacion sobre los asentamientos para un archivo de guardado
 */
USTRUCT(BlueprintType)
struct FSettlementSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	int32 Owner;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	float HealthPoints;

	UPROPERTY(SaveGame, VisibleInstanceOnly, BlueprintReadWrite, Category="Saves")
	FSettlementInfo Info;

	FSettlementSaveData(): FSettlementSaveData(-1, 0.0, FSettlementInfo())
	{
	}

	FSettlementSaveData(const int32 Owner, const float HealthPoints, const FSettlementInfo& Info)
		: Owner(Owner),
		  HealthPoints(HealthPoints),
		  Info(Info)
	{
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAttackStats.h"
#include "FResourceInfo.h"
#include "FTileInfo.h"
#include "Engine/DataTable.h"
#include "FUnitInfo.h"
#include "FDataTableStructures.generated.h"

USTRUCT(BlueprintType)
struct FIconData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D Translation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D Scale;

	FIconData(): FIconData(nullptr, FVector2D(0.0), FVector2D(0.0))
	{
	}

	FIconData(UTexture2D* const Image, const FVector2D& Translation, const FVector2D& Scale)
		: Image(Image),
		  Translation(Translation),
		  Scale(Scale)
	{
	}
};

USTRUCT(BlueprintType)
struct FTileData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETileType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MovementCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIconData Icon;

	FTileData(): FTileData(TEXT(""), ETileType::None, 0, FIconData())
	{
	}

	FTileData(const FString& Name, const ETileType Type, const int32 MovementCost, const FIconData& IconData)
		: Name(Name),
		  Type(Type),
		  MovementCost(MovementCost),
		  Icon(IconData)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FResourceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FResource Resource;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIconData Icon;

	FResourceData(): FResourceData(TEXT(""), FResource(), FIconData())
	{
	}

	FResourceData(const FString& Name, const FResource& Resource, const FIconData& Icon)
		: Name(Name),
		  Resource(Resource),
		  Icon(Icon)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Estructura que almacena informacion sobre los atributos de la unidad que deben ser almacenados en
 * una tabla de datos (almacenamiento permanente)
 */
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * Nombre de la unidad
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Name;

	/**
	 * Tipo de unidad. 'None' si es una unidad civil
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUnitType Type;

	/**
	 * Puntos de movimiento
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MovementPoints;

	/**
	 * Puntos de visibilidad
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 VisibilityPoints;

	/**
	 * Coste de produccion
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProductionCost;

	/**
	 * Coste de mantenimiento
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaintenanceCost;

	/**
	 * Puntos de vida
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealthPoints;

	/**
	 * Estadisticas de ataque
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FAttackStats Stats;

	/**
	 * Informacion sobre el icono que representa la unidad
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIconData Icon;

	FUnitData(): FUnitData(TEXT(""), EUnitType::None, 0, 0, 0.0, 0.0,
	                       0.0, FAttackStats(), FIconData())
	{
	}

	FUnitData(const FString& Name, const EUnitType Type, const int32 MovementPoints, const int32 VisibilityPoints,
		const float ProductionCost, const float MaintenanceCost, const float HealthPoints, const FAttackStats& Stats,
		const FIconData& Icon)
		: Name(Name),
		  Type(Type),
		  MovementPoints(MovementPoints),
		  VisibilityPoints(VisibilityPoints),
		  ProductionCost(ProductionCost),
		  MaintenanceCost(MaintenanceCost),
		  HealthPoints(HealthPoints),
		  Stats(Stats),
		  Icon(Icon)
	{
	}
};

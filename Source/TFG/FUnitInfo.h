// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMovement.h"
#include "FUnitInfo.generated.h"

/**
 * Tipo enumerado para determinar el tipo de unidad
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Civil = 1 UMETA(DisplayName"Civil unit"),
	Infantry = 2 UMETA(DisplayName="Infantry unit"),
	Armoured = 3 UMETA(DisplayName="Armoured unit"),
	AntiTank = 4 UMETA(DisplayName="Anti-tank unit")
};

/**
 * Tipo enumerado para determinar el estado de la unidad
 */
UENUM(BlueprintType)
enum class EUnitState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	WaitingForOrders = 1 UMETA(DisplayName="WaitingForOrders"),
	Sleeping = 2 UMETA(DisplayName="Sleeping"),
	FollowingPath = 3 UMETA(DisplayName="FollowingPath"),
	NoMovementPoints = 4 UMETA(DisplayName="NoMovementPoints"),
	WaitingForNextTurn = 5 UMETA(DisplayName="WaitingForNextTurn")
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Estructura que almacena informacion sobre los atributos de la unidad
 */
USTRUCT(BlueprintType)
struct FUnitInfo
{
	GENERATED_BODY()

	/**
	 * Posicion en el Array2D de casillas
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	FIntPoint Pos2D;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Military")
	EUnitType Type;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Puntos de movimiento base
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	int32 BaseMovementPoints;
	/**
	 * Puntos de movimiento en un instante concreto
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	int32 MovementPoints;

	/**
	 * Camino a seguir
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info|Path")
	TArray<FMovement> Path;
	/**
	 * Casillas completadas del camino en el turno actual
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info|Path")
	TArray<FMovement> PathCompleted;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Puntos de visibilidad
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	int32 VisibilityPoints;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Coste de mantenimiento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	float MaintenanceCost;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Estado actual
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	EUnitState State;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Constructor por defecto
	 */
	FUnitInfo(): FUnitInfo(FIntPoint(-1), EUnitType::None, 2, TArray<FMovement>(),
	                       2, 2.0, EUnitState::None)
	{
	}

	/**
	 * Constructor con parametros
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 * @param UnitType Tipo de unidad. 'None' si es una unidad civil
	 * @param UnitBaseMovP Movimiento base de la unidad
	 * @param UnitPath Camino asignado
	 * @param UnitVisP Puntos de visibilidad
	 * @param UnitMaintC Coste de mantenimiento
	 * @param UnitState Estado de la unidad
	 */
	FUnitInfo(const FIntPoint& Pos, const EUnitType UnitType, const int32 UnitBaseMovP,
	          const TArray<FMovement>& UnitPath, const int32 UnitVisP, const float UnitMaintC,
	          const EUnitState UnitState)
	{
		Pos2D = Pos;
		Type = UnitType;
		BaseMovementPoints = MovementPoints = UnitBaseMovP;

		Path = UnitPath;
		PathCompleted = TArray<FMovement>();

		VisibilityPoints = UnitVisP;
		MaintenanceCost = UnitMaintC;

		State = UnitState;
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMovement.h"
#include "FUnitInfo.generated.h"

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

	/**
	 * Puntos de movimiento base
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Info")
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
	 * Coste de produccion
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	float ProductionCost;
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

	FUnitInfo(): FUnitInfo(FIntPoint(-1), 2, TArray<FMovement>(), 2, 100.0, 2.0, EUnitState::None) {}

	FUnitInfo(const FIntPoint& Pos, const int32 UnitBaseMovP, const TArray<FMovement>& UnitPath, const int32 UnitVisP, const float UnitProdC, const float UnitMaintC, const EUnitState UnitState)
	{
		Pos2D = Pos;
		BaseMovementPoints = MovementPoints = UnitBaseMovP;

		Path = UnitPath;
		PathCompleted = TArray<FMovement>();

		VisibilityPoints = UnitVisP;
		ProductionCost = UnitProdC;
		MaintenanceCost = UnitMaintC;

		State = UnitState;
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAttackStats.generated.h"

/**
 * Estructura que almacena informacion sobre los atributos de ataque y defensa de un elemento del juego
 */
USTRUCT(BlueprintType)
struct FAttackStats
{
	GENERATED_BODY()

	/**
	 * Puntos de ataque
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameElement|Attack")
	float AttackPoints;

	/**
	 * Puntos de defensa
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameElement|Attack")
	float DefensePoints;

	//----------------------------------------------------------------------------------------------------------------//

	FAttackStats(): FAttackStats(0, 0)
	{
	}

	FAttackStats(const float Attack, const float Defense)
	{
		AttackPoints = Attack;
		DefensePoints = Defense;
	}
};

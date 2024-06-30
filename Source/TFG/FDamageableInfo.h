// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAttackStats.h"
#include "FDamageableInfo.generated.h"

USTRUCT(BlueprintType)
struct FDamageableInfo
{
	GENERATED_BODY()

	/**
	 * Puntos de vida base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseHealthPoints;
	/**
	 * Puntos de vida del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float HealthPoints;

	/**
	 * Estadisticas base de ataque del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	FAttackStats BaseStats;

	/**
	 * Estadisticas de ataque del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	FAttackStats Stats;

	FDamageableInfo(): FDamageableInfo(100.0, FAttackStats(10.0, 10.0)) {}

	FDamageableInfo(const float BaseHealthP, const FAttackStats& BaseS)
	{
		BaseHealthPoints = HealthPoints = BaseHealthP;
		BaseStats = Stats = BaseS;
	}
};

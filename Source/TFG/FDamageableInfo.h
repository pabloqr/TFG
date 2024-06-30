// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	 * Puntos de ataque base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseAttackPoints;
	/**
	 * Puntos de defensa base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseDefensePoints;
	
	/**
	 * Puntos de vida del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float HealthPoints;
	/**
	 * Puntos de ataque del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float AttackPoints;
	/**
	 * Puntos de defensa del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float DefensePoints;

	FDamageableInfo(): FDamageableInfo(100.0, 10.0, 10.0) {}

	FDamageableInfo(const float BaseHealthP, const float BaseAttackP, const float BaseDefP)
	{
		BaseHealthPoints = HealthPoints = BaseHealthP;
		BaseAttackPoints = AttackPoints = BaseAttackP;
		BaseDefensePoints = DefensePoints = BaseDefP;
	}
};

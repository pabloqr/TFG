// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAttackStats.generated.h"

USTRUCT(BlueprintType)
struct FAttackStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="GameElement|Attack")
	float AttackPoints;
	UPROPERTY(BlueprintReadWrite, Category="GameElement|Attack")
	float DefensePoints;

	FAttackStats(): FAttackStats(0, 0) {}

	FAttackStats(const float Attack, const float Defense)
	{
		AttackPoints = Attack;
		DefensePoints = Defense;
	}
};

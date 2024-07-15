// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorMilitaryUnit.h"
#include "FProductionElement.generated.h"

class AActorMilitaryUnit;
class AActorUnit;

USTRUCT(BlueprintType)
struct FProductionElement
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ProductionElement")
	TSubclassOf<AActorUnit> Unit;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ProductionElement")
	EUnitType UnitType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ProductionElement")
	float ProductionCost;

	FProductionElement(): FProductionElement(TSubclassOf<AActorUnit>(), EUnitType::None, 0)
	{
	}

	FProductionElement(const TSubclassOf<AActorUnit>& Unit, const EUnitType UnitType, const float ProductionCost)
		: Unit(Unit),
		  UnitType(UnitType),
		  ProductionCost(ProductionCost)
	{
	}
};

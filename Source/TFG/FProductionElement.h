// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	int32 ProductionCost;

	FProductionElement(): FProductionElement(TSubclassOf<AActorUnit>(), 0) {}
	
	FProductionElement(const TSubclassOf<AActorUnit> UnitToProduce, const int32 Cost)
	{
		Unit = UnitToProduce;
		ProductionCost = Cost;
	}
};

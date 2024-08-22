// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FResourceInfo.h"
#include "FDealInfo.generated.h"

USTRUCT(BlueprintType)
struct FDealInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	float Money;

	// UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	// float MoneyPerTurn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	FResource Resource;

	FDealInfo(): FDealInfo(0.0, FResource())
	{
	}

	FDealInfo(const float Money, const FResource& Resource)
		: Money(Money),
		  Resource(Resource)
	{
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FResourceInfo.h"
#include "FDealInfo.generated.h"

UENUM(BlueprintType)
enum class EDealType : uint8
{
	WarDeal = 0 UMETA(DisplayName = "WarDeal"),
	AllianceDeal = 1 UMETA(DisplayName = "AllianceDeal"),
	ExchangeDeal = 2 UMETA(DisplayName = "ExchangeDeal"),
};

USTRUCT(BlueprintType)
struct FDealElements
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	int32 FactionIndex;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	float Money;

	// UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	// float MoneyPerTurn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	FResource Resource;

	FDealElements(): FDealElements(-1, 0.0, FResource())
	{
	}

	FDealElements(const int32 FactionIndex, const float Money, const FResource& Resource)
		: FactionIndex(FactionIndex),
		  Money(Money),
		  Resource(Resource)
	{
	}
};

USTRUCT(BlueprintType)
struct FDealInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	EDealType Type;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	FDealElements FactionAElements;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DealInfo")
	FDealElements FactionBElements;

	FDealInfo(): FDealInfo(EDealType::ExchangeDeal, FDealElements(), FDealElements())
	{
	}

	FDealInfo(const EDealType Type, const FDealElements& FactionAElements, const FDealElements& FactionBElements)
		: Type(Type),
		  FactionAElements(FactionAElements),
		  FactionBElements(FactionBElements)
	{
	}
};

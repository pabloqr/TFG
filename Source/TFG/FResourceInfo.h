// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FResourceInfo.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Strategic = 1 UMETA(DisplayName="Strategic"),
	Monetary = 2 UMETA(DisplayName="Monetary")
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FResourceInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	FIntPoint Pos2D;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	EResourceType Type;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	int32 Quantity;

	FResourceInfo(): FResourceInfo(FIntPoint(-1), EResourceType::None, 0.0)
	{
	}

	FResourceInfo(const FIntPoint& Pos, const EResourceType ResourceType, const int32 ResourceQuantity)
	{
		Pos2D = Pos;

		Type = ResourceType;
		Quantity = ResourceQuantity;
	}
};

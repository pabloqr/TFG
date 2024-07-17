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

UENUM(BlueprintType, meta=(ScriptName="GameResource"))
enum class EResource : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Diamond = 1 UMETA(DisplayName="Diamond"),
	Gold = 2 UMETA(DisplayName="Gold"),
	Copper = 3 UMETA(DisplayName="Copper"),
	Aluminium = 4 UMETA(DisplayName="Aluminium"),
	Coal = 5 UMETA(DisplayName="Coal"),
	Oil = 6 UMETA(DisplayName="Oil")
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FResource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	EResource Resource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	EResourceType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	int32 Quantity;

	FResource(): FResource(EResource::None, EResourceType::None, 0)
	{
	}

	FResource(const EResource Resource, const EResourceType Type, const int32 Quantity)
		: Resource(Resource),
		  Type(Type),
		  Quantity(Quantity)
	{
	}
};

USTRUCT(BlueprintType)
struct FResourceInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	FIntPoint Pos2D;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	int32 Owner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	FResource Resource;

	FResourceInfo(): FResourceInfo(FIntPoint(-1), -1, FResource())
	{
	}

	FResourceInfo(const FIntPoint& Pos2D, const int32 Owner, const FResource& Resource)
		: Pos2D(Pos2D),
		  Owner(Owner),
		  Resource(Resource)
	{
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FResourceInfo.h"
#include "FWarInfo.h"
#include "UObject/Object.h"
#include "FFactionInfo.generated.h"

enum class EResource : uint8;
class AActorUnit;
class AActorSettlement;

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FFactionsSet
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="FactionsArray")
	TSet<int32> Factions;

	FFactionsSet(): FFactionsSet(TSet<int32>())
	{
	}

	explicit FFactionsSet(const TSet<int32>& Factions)
		: Factions(Factions)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FResourceCollection
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	FResource GatheredResource;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	TArray<FIntPoint> Tiles;

	FResourceCollection(): FResourceCollection(FResource(), TArray<FIntPoint>())
	{
	}

	FResourceCollection(const FResource& GatheredResource, const TArray<FIntPoint>& Tiles)
		: GatheredResource(GatheredResource),
		  Tiles(Tiles)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

UENUM(BlueprintType)
enum class EDiplomaticRelationship : uint8
{
	AtWar = 0 UMETA(DisplayName = "AtWar"),
	Neutral = 1 UMETA(DisplayName = "Neutral"),
	Ally = 2 UMETA(DisplayName = "Ally"),
};

USTRUCT(BlueprintType)
struct FOpponentFactionInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="OpponentFactionInfo")
	EDiplomaticRelationship Relationship;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="OpponentFactionInfo")
	float MilitaryStrength;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="OpponentFactionInfo")
	FWarInfo WarInfo;

	FOpponentFactionInfo(): FOpponentFactionInfo(EDiplomaticRelationship::Neutral, 0.0, FWarInfo())
	{
	}

	FOpponentFactionInfo(const EDiplomaticRelationship Relationship, const float MilitaryStrength,
	                     const FWarInfo& WarInfo)
		: Relationship(Relationship),
		  MilitaryStrength(MilitaryStrength),
		  WarInfo(WarInfo)
	{
	}
};

//--------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FFactionInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	int32 Index;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float MilitaryStrength;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float Money;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float MoneyBalance;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorSettlement*> Settlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> IdleSettlements;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorUnit*> Units;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> CivilUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> MilitaryUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> ManualUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> AutomaticUnits;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> MonetaryResources;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> StrategicResources;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Opponents")
	TMap<int32, FOpponentFactionInfo> KnownFactions;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Opponents")
	TMap<EDiplomaticRelationship, FFactionsSet> FactionsWithDiplomaticRelationship;

	FFactionInfo(): FFactionInfo(-1, 0.0, 0.0, 0.0, {}, {},
	                             {}, {}, {}, {}, {},
	                             {}, {}, {}, {})
	{
	}

	FFactionInfo(const int32 Index, const float MilitaryStrength, const float Money, const float MoneyBalance,
	             const TArray<AActorSettlement*>& Settlements, const TArray<int32>& IdleSettlements,
	             const TArray<AActorUnit*>& Units, const TArray<int32>& CivilUnits, const TArray<int32>& MilitaryUnits,
	             const TArray<int32>& ManualUnits, const TArray<int32>& AutomaticUnits,
	             const TMap<EResource, FResourceCollection>& MonetaryResources,
	             const TMap<EResource, FResourceCollection>& StrategicResources,
	             const TMap<int32, FOpponentFactionInfo>& KnownFactions,
	             const TMap<EDiplomaticRelationship, FFactionsSet>& FactionsWithDiplomaticRelationship)
		: Index(Index),
		  MilitaryStrength(MilitaryStrength),
		  Money(Money),
		  MoneyBalance(MoneyBalance),
		  Settlements(Settlements),
		  IdleSettlements(IdleSettlements),
		  Units(Units),
		  CivilUnits(CivilUnits),
		  MilitaryUnits(MilitaryUnits),
		  ManualUnits(ManualUnits),
		  AutomaticUnits(AutomaticUnits),
		  MonetaryResources(MonetaryResources),
		  StrategicResources(StrategicResources),
		  KnownFactions(KnownFactions),
		  FactionsWithDiplomaticRelationship(FactionsWithDiplomaticRelationship)
	{
	}
};

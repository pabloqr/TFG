// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorTileMap.h"
#include "Engine/GameInstance.h"
#include "GInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileMapUpdated);

//--------------------------------------------------------------------------------------------------------------------//

/**
 * 
 */
UCLASS()
class TFG_API UGInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Game")
	int32 NumFactions;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Map")
	FIntPoint Size2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Map")
	FVector2D MapSize2D;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Map")
	EMapTemperature MapTemperature;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Map")
	EMapSeaLevel MapSeaLevel;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameInstance|Map")
	float WaterTileChance;

	//----------------------------------------------------------------------------------------------------------------//

	virtual void Init() override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnTileMapUpdated OnTileMapUpdated;
};

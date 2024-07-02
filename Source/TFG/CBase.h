// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTileInfo.h"
#include "GameFramework/PlayerController.h"
#include "CBase.generated.h"

class AActorTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedTileUpdated, ETileType, TileType);

/**
 * 
 */
UCLASS()
class TFG_API ACBase : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorTile* HoveredTile;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorTile* SelectedTile;

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSelectedTileUpdated OnSelectedTileUpdated;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CBase.generated.h"

class AActorTile;
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
};

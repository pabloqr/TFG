// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "ActorSettlement.h"
#include "GameFramework/PlayerController.h"
#include "CMain.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API ACMain : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorTile* HoveredTile;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorTile* SelectedTile;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorUnit* Unit;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorSettlement* Settlement;

	//----------------------------------------------------------------------------------------------------------------//
	
	virtual void BeginPlay() override;
};

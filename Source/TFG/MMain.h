// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MMain.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API AMMain : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int32 MaxNumFactions;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int32 MaxTurns;
};

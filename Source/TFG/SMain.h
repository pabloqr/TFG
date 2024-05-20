// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FElementIdentifier.h"
#include "GameFramework/GameStateBase.h"
#include "SMain.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API ASMain : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainState")
	TArray<FElementIdentifier> FactionsAlive;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int32 CurrentFaction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int64 Turn;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBase.h"
#include "GameFramework/PlayerController.h"
#include "CMain.generated.h"

class AActorSettlement;
class AActorUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedSettlementUpdated, AActorSettlement*, Settlement);

/**
 * 
 */
UCLASS()
class TFG_API ACMain : public ACBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorUnit* Unit;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorSettlement* Settlement;

	//----------------------------------------------------------------------------------------------------------------//

	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSelectedSettlementUpdated OnSelectedSettlementUpdated;
};

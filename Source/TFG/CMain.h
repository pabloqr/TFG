// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBase.h"
#include "InterfaceDeal.h"
#include "GameFramework/PlayerController.h"
#include "CMain.generated.h"

class AActorSettlement;
class AActorUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedUnitUpdated, AActorUnit*, Unit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedSettlementUpdated, AActorSettlement*, Settlement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDealProposed, FDealInfo, Deal);

/**
 * 
 */
UCLASS()
class TFG_API ACMain : public ACBase, public IInterfaceDeal
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorUnit* Unit;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ControllerMain")
	AActorSettlement* Settlement;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ProposeDeal(const FDealInfo& Deal) const override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSelectedUnitUpdated OnSelectedUnitUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSelectedSettlementUpdated OnSelectedSettlementUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnDealProposed OnDealProposed;
};

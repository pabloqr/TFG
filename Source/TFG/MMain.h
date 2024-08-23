// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDealInfo.h"
#include "GameFramework/GameModeBase.h"
#include "MMain.generated.h"

class ASMain;

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitFinished);

/**
 * 
 */
UCLASS()
class TFG_API AMMain : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	ASMain* State;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int32 MaxNumFactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MainMode")
	int32 MaxTurns;

private:
	void MakePeaceWithFaction(const FDealInfo& Deal) const;

	void MakeAllianceWithFaction(const FDealInfo& Deal) const;

	void MakeExchangeDeal(const FDealInfo& Deal) const;

protected:
	virtual void BeginPlay() override;

public:
	TMap<int32, float> GetFactionsMilitaryStrength() const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	float CalculateMoneyAmountForPeaceTreaty(int32 TargetFaction, const bool ImWeaker,
	                                         const float WarScore, float StrengthDiffRel) const;

	UFUNCTION(BlueprintCallable)
	float CalculateMoneyAmountForAllianceTreaty(int32 TargetFaction, float StrengthDiffRel) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	bool IsFactionAtWarWith(const int32 TargetFaction, const int32 FactionAtWar) const;

	UFUNCTION(BlueprintCallable)
	bool IsFactionNeutralWith(const int32 TargetFaction, const int32 NeutralFaction) const;

	UFUNCTION(BlueprintCallable)
	bool IsFactionAlliedWith(const int32 TargetFaction, const int32 AllyFaction) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void ProposeDeal(const FDealInfo& Deal) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void ResolveDeal(float DealResult, const FDealInfo& Deal) const;

	UFUNCTION(BlueprintCallable)
	void DeclareWarOnFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	float GetWarScore(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	float GetWarTurns(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	void BreakAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void NextTurn() const;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnInitFinished OnInitFinished;
};

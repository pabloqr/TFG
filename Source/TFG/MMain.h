// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MMain.generated.h"

class ASMain;
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

	//----------------------------------------------------------------------------------------------------------------//

	virtual void BeginPlay() override;

public:
	TMap<int32, float> GetFactionsMilitaryStrength() const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void DeclareWarOnFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	void MakePeaceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	void MakeAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	UFUNCTION(BlueprintCallable)
	void BreakAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void NextTurn() const;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnInitFinished OnInitFinished;
};

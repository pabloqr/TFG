// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "FCivilUnitInfo.h"
#include "GameFramework/Actor.h"
#include "ActorCivilUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceGathered, const FIntPoint&, Pos2D);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettlementCreated, const FIntPoint&, Pos2D);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API AActorCivilUnit : public AActorUnit
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	FCivilUnitInfo CivilInfo;

public:
	// Sets default values for this actor's properties
	AActorCivilUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	const FIntPoint& GetTargetPos() const { return CivilInfo.TargetPos; }
	ECivilUnitState GetCivilUnitState() const { return CivilInfo.State; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetTargetPos(const FIntPoint& Pos) { CivilInfo.TargetPos = Pos; }
	void SetCivilUnitState(const ECivilUnitState State) { CivilInfo.State = State; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	bool CanSetSettlement() const;

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void GatherResource();

	UFUNCTION(BlueprintCallable)
	void CreateSettlement();

	//----------------------------------------------------------------------------------------------------------------//

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnResourceGathered OnResourceGathered;

	UPROPERTY(BlueprintAssignable)
	FOnSettlementCreated OnSettlementCreated;
};

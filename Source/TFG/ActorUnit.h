// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "ActorTileMap.h"
#include "FMovement.h"
#include "GameFramework/Actor.h"
#include "ActorUnit.generated.h"

UENUM()
enum class EUnitState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	WaitingForOrders = 1 UMETA(DisplayName="WaitingForOrders"),
	Sleeping = 2 UMETA(DisplayName="Sleeping"),
	FollowingPath = 3 UMETA(DisplayName="FollowingPath"),
	NoMovementPoints = 4 UMETA(DisplayName="NoMovementPoints")
};

UCLASS(Abstract)
class TFG_API AActorUnit : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	EUnitState State;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Unit")
	int32 BaseMovementPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	int32 MovementPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Path")
	TArray<FMovement> Path;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	int32 VisibilityPoints;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	float ProductionCost;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	float MaintenanceCost;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|AditionalData")
	AActorTileMap* TileMap;

public:
	// Sets default values for this actor's properties
	AActorUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	EUnitState GetState() const { return State; }
	float GetMaintenanceCost() const { return MaintenanceCost; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AssignPath(const TArray<FIntPoint>& Tiles, const TArray<int32>& Costs);
	
	UFUNCTION(BlueprintCallable)
	void ContinuePath();
	
	UFUNCTION(BlueprintCallable)
	bool MoveUnit();
	
	UFUNCTION(BlueprintCallable)
	void RestoreMovement();

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnEnded();

	//----------------------------------------------------------------------------------------------------------------//
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------------------------------------------------//

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitMoved, FIntPoint, PrevPos, FIntPoint, CurrPos);

	UPROPERTY(BlueprintAssignable)
	FOnUnitMoved OnUnitMoved;
};

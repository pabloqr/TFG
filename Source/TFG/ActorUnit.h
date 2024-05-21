// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "GameFramework/Actor.h"
#include "ActorUnit.generated.h"

UENUM(BlueprintType)
enum class EMovement : uint8
{
	None = 0 UMETA(DisplayName="None"),
	NorthWest = 1 UMETA(DisplayName="NW"),
	North = 2 UMETA(DisplayName="N"),
	NorthEast = 3 UMETA(DisplayName="NE"),
	SouthEast = 4 UMETA(DisplayName="SE"),
	South = 5 UMETA(DisplayName="S"),
	SouthWest = 6 UMETA(DisplayName="SW"),
};

UENUM()
enum class EUnitState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	WaitingForOrders = 1 UMETA(DisplayName="WaitingForOrders"),
	Sleeping = 2 UMETA(DisplayName="Sleeping"),
	FollowingPath = 3 UMETA(DisplayName="FollowingPath"),
	NoMovementPoints = 4 UMETA(DisplayName="NoMovementPoints")
};

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Movement")
	int32 MovementCost;

	FMovement() : FMovement(FIntPoint(-1, -1), -1) {}
	
	FMovement(const FIntPoint& Pos, const int32 Cost)
	{
		Pos2D = Pos;
		MovementCost = Cost;
	}
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

public:
	// Sets default values for this actor's properties
	AActorUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	EUnitState GetState() const { return State; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AssignPath(const TArray<FIntPoint>& Tiles, const TArray<int32>& Costs);
	
	UFUNCTION(BlueprintCallable)
	void ContinuePath();
	
	UFUNCTION(BlueprintCallable)
	bool MoveUnit(const FMovement& Move, const bool ManualMove);
	
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUnitMoved, FIntPoint, PrevPos, FIntPoint, CurrPos, AActorUnit*, Unit);

	UPROPERTY(BlueprintAssignable)
	FOnUnitMoved OnUnitMoved;
};

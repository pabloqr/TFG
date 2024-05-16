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

UCLASS(Abstract)
class TFG_API AActorUnit : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Unit")
	int32 BaseMovementPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	int32 MovementPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Path")
	TArray<FIntPoint> CurrentPath;
	
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

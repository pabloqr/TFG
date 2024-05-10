// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "GameFramework/Actor.h"
#include "ActorUnit.generated.h"

USTRUCT(BlueprintType)
struct FUnitInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	FString Name;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	float ProductionCost;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	float HealthPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	float DefensePoints;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	int32 MovementPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Info")
	int32 VisibilityPoints;
};

UCLASS(Abstract)
class TFG_API AActorUnit : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	FUnitInfo UnitInfo;

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

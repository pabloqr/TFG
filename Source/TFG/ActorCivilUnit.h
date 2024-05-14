// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "GameFramework/Actor.h"
#include "ActorCivilUnit.generated.h"

UCLASS()
class TFG_API AActorCivilUnit : public AActorUnit
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Civil")
	int32 NumActions;

public:
	// Sets default values for this actor's properties
	AActorCivilUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

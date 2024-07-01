// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "FResourceInfo.h"
#include "GameFramework/Actor.h"
#include "ActorMilitaryUnit.generated.h"

UCLASS()
class TFG_API AActorMilitaryUnit : public AActorUnit
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Military")
	EResourceType ProductionResource;

public:
	// Sets default values for this actor's properties
	AActorMilitaryUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

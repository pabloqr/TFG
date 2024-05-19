// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorCivilUnit.h"
#include "ActorMilitaryUnit.h"
#include "ActorTileMap.h"
#include "GameFramework/Actor.h"
#include "ActorGameManager.generated.h"

UCLASS()
class TFG_API AActorGameManager : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameManager|Map")
	AActorTileMap* TileMap;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameManager|Units")
	TArray<AActorMilitaryUnit*> MilitaryUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameManager|Units")
	TArray<AActorCivilUnit*> CivilUnits;

public:
	// Sets default values for this actor's properties
	AActorGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorTileGrid.generated.h"

UCLASS()
class TFG_API AActorTileGrid : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Map|Interaction")
	UInstancedStaticMeshComponent *InstancedStaticMeshComponent;

	TArray<FIntPoint> Positions;

public:
	// Sets default values for this actor's properties
	AActorTileGrid();

protected:
	void AddTileInstance(const FVector2D MapPos2D, FIntPoint Pos2D);
	void RemoveTileInstance(FIntPoint Pos2D);
	void ClearTileInstances();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class ETileType : uint8
{
	None,
	Plains,
	Hills,
	Forest,
	Snow,
	Ice,
	Mountains,
	Water,
	Max UMETA(Hidden)
};

UCLASS()
class TFG_API ATile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile")
	FIntPoint MapPosition;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	ETileType TileType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent *TileMesh;
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	FIntPoint GetMapPosition() const;

	void SetPosition(const FIntPoint& Position);
};

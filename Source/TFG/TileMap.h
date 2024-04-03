// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMap.generated.h"

enum class ETileType;
class ATile;

struct FSTileProbability
{
	float PlainsProbability = 0.f;
	float HillsProbability = 0.f;
	float ForestProbability = 0.f;
	float SnowProbability = 0.f;
	float IceProbability = 0.f;
	float MountainsProbability = 0.f;
	float WaterProbability = 0.f;
};

UCLASS()
class TFG_API ATileMap : public AActor
{
	GENERATED_BODY()

protected:
	TArray<ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float RowOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float HorizontalOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float VerticalOffset;

	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> PlainsTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> HillsTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> ForestTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> SnowTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> IceTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> MountainsTile;
	UPROPERTY(EditAnywhere, Category="Map|Tile")
	TSubclassOf<ATile> WaterTile;

	UPROPERTY(EditAnywhere, Category="Map|Parameters")
	float WaterTileChance = 0.3f;
	
public:	
	// Sets default values for this actor's properties
	ATileMap();

private:
	int32 GetPositionInArray(const int32 Row, const int32 Col) const;
	int32 GetPositionInArray(const FIntPoint& Pos) const;

	TSubclassOf<ATile> GenerateTileType(int32 Row, int32 Col, TArray<FSTileProbability> &Probabilities);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
};

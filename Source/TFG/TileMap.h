// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMap.generated.h"

enum class ETileType : uint8;
class ATile;

UENUM(BlueprintType)
enum class EMapTemperature : uint8
{
	Desert = 0 UMETA(DisplayName="Desert"),
	Hot = 1 UMETA(DisplayName="Hot"),
	Temperate = 2 UMETA(DisplayName="Temperate"),
	Cold = 3 UMETA(DisplayName="Cold"),
};

UENUM(BlueprintType)
enum class EMapSeaLevel : uint8
{
	Arid = 0 UMETA(DisplayName="Arid"),
	Standard = 1 UMETA(DisplayName="Standard"),
	Wet = 2 UMETA(DisplayName="Wet"),
};

struct FSTileProbability
{
	float PlainsProbability = 0.f;
	float HillsProbability = 0.f;
	float ForestProbability = 0.f;
	float SnowProbability = 0.f;
	float IceProbability = 0.f;
	float MountainsProbability = 0.f;
	float WaterProbability = 0.f;
	
	int Error = 0;
};

UCLASS()
class TFG_API ATileMap : public AActor
{
	GENERATED_BODY()

protected:
	TArray<ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Rows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Cols;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float RowOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float HorizontalOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float VerticalOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> PlainsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> HillsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> ForestTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> SnowTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> IceTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> MountainsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<ATile> WaterTile;

	UPROPERTY(VisibleAnywhere, Category="Map|Parameters")
	float WaterTileChance = 0.4f;
	float WaterProbabilityModifier;
	
	UPROPERTY(EditAnywhere, Category="Map|Parameters")
	EMapTemperature MapTemperature;
	UPROPERTY(EditAnywhere, Category="Map|Parameters")
	EMapSeaLevel MapSeaLevel;

	int32 NumIceRows;
	int32 NumSnowRows;
	
public:	
	// Sets default values for this actor's properties
	ATileMap();

private:
	int32 GetPositionInArray(const int32 Row, const int32 Col) const;
	int32 GetPositionInArray(const FIntPoint& Pos) const;

	FIntPoint GetCoordsInMap(const int32 Pos) const;
	int32 GetRowInMap(const int32 Pos) const;
	int32 GetColInMap(const int32 Pos) const;

	float ProbabilityOfIce(const int32 Pos, int32 &IceRow) const;

	void UpdateProbability(const FIntPoint &Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability> &Probabilities) const;
	void UpdateProbabilityAtPos(const FIntPoint &Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability> &Probabilities) const;

	TSubclassOf<ATile> GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FSTileProbability> &Probabilities) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
};

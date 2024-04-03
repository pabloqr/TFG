// Fill out your copyright notice in the Description page of Project Settings.

#include "TileMap.h"
#include "Tile.h"

// Sets default values
ATileMap::ATileMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
}

//--------------------------------------------------------------------------------------------------------------------//

int32 ATileMap::GetPositionInArray(const int32 Row, const int32 Col) const
{
	return Row * Width + Col;
}

int32 ATileMap::GetPositionInArray(const FIntPoint& Pos) const
{
	return Pos.X * Width + Pos.Y;
}

TSubclassOf<ATile> ATileMap::GenerateTileType(int32 Row, int32 Col, TArray<FSTileProbability>& Probabilities)
{
	TSubclassOf<ATile> GeneratedTile;

	
	
	return GeneratedTile;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void ATileMap::BeginPlay()
{
	Super::BeginPlay();
	
	const int32 Dimension = Height*Width;
	TArray<FSTileProbability> Probabilities;
	
	Tiles.SetNumZeroed(Dimension);
	Probabilities.SetNumZeroed(Dimension);

	for (int32 Row = 0; Row < Height; ++Row)
	{
		for (int32 Col = 0; Col < Width; ++Col)
		{
			const float RowPos = Col * HorizontalOffset;
			const float ColPos = (Col % 2 == 0) ? Row * VerticalOffset : Row * VerticalOffset + RowOffset;

			const int32 ArrayPosition = GetPositionInArray(Row, Col);
			
			TSubclassOf<ATile> TileToSpawn = GenerateTileType(Row, Col, Probabilities);

			ATile *NewTile = GetWorld()->SpawnActor<ATile>(TileToSpawn, FVector(FIntPoint(RowPos, ColPos)), FRotator::ZeroRotator);
			NewTile->SetPosition(FIntPoint(Row, Col));
			NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), Row, Col));

			Tiles[ArrayPosition] = NewTile;
		}
	}
}

// Called every frame
// void ATileMap::Tick(float DeltaTime)
// {
//	Super::Tick(DeltaTime);

// }


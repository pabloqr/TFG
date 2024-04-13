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

/**
 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
 * 
 * @param Row Indice de la fila
 * @param Col Indice de la columna
 * @return Posicion en el Array1D
 */
int32 ATileMap::GetPositionInArray(const int32 Row, const int32 Col) const
{
	return Row * Cols + Col;
}

/**
 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
 * 
 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @return Posicion en el Array1D
 */
int32 ATileMap::GetPositionInArray(const FIntPoint& Pos) const
{
	return Pos.X * Cols + Pos.Y;
}

/**
 * Metodo privado que obtiene las coordenadas dentro del Array2D dada su posicion en el Array1D
 * 
 * @param Pos Posicion en el Array1D
 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 */
FIntPoint ATileMap::GetCoordsInMap(const int32 Pos) const
{
	return FIntPoint(GetRowInMap(Pos), GetColInMap(Pos));
}

/**
 * Metodo privado que obtiene la coordenada de la fila en el Array2D
 * 
 * @param Pos Posicion en el Array1D
 * @return Valor de la fila en el Array2D
 */
int32 ATileMap::GetRowInMap(const int32 Pos) const
{
	return Pos / Cols;
}

/**
 * Metodo privado que obtiene la coordenada de la columna en el Array2D
 * 
 * @param Pos Posicion en el Array1D
 * @return Valor de la columna en el Array2D
 */
int32 ATileMap::GetColInMap(const int32 Pos) const
{
	return Pos % Cols;
}

/**
 * Metodo que calcula la probabilidad de que una casilla sea Hielo (IceTile), se hara para que se acumule
 * en los polos
 * 
 * @param Pos Posicion en el Array1D
 * @param IceRow Indice dentro del numero de filas que pueden contener Hielo
 * @return Probabilidad de que la casilla en la posicion dada pueda contener Hielo
 */
float ATileMap::ProbabilityOfIce(const int32 Pos, int32 &IceRow) const
{
	float CurrentProbability = 0.8;
	const int32 Row = GetRowInMap(Pos);
	const int32 Col = GetColInMap(Pos);

	// Se comprueba si la casilla es la primera de la fila para actualizar el indice
	if (Col == 0)
	{
		IceRow += Row <= NumIceRows ? 1 : Row >= Rows-NumIceRows ? -1 : 0;
	}
	// Si la casilla esta fuera del rango de casillas que pueden contener Hielo, la probabilidad es 0
	// En caso contrario, se verifica el numero de filas que pueden contener Hielo para aplicar ajustes
	if (IceRow >= NumIceRows) CurrentProbability = 0.0;
	else if (NumIceRows == 1)
	{
		// Si el numero de filas que pueden contener Hielo es 1, limitaremos la aparicion de este tipo de casillas
		// A aquellas que se situen en el limite exterior del mapa dado que el tamano del mapa es pequeno
		const bool CorrectPos = (Row < NumIceRows && Col%2 != 0) || (Row >= Rows-NumIceRows && Col%2 == 0);
		CurrentProbability = CorrectPos ? 0.0 : CurrentProbability;
	}
	
	for (int32 i = 0; i < IceRow && IceRow > 0 && IceRow < NumIceRows; ++i)
	{
		CurrentProbability *= FMath::Pow(0.1 / CurrentProbability, 1.f / NumIceRows);
	}
	
	return CurrentProbability;
}

/**
 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en las casillas
 * circundantes a la actual
 * 
 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @param TileType Tipo de casilla a modificar
 * @param Probability Variacion en el valor de la probabilidad
 * @param Probabilities Array de probabilidades
 */
void ATileMap::UpdateProbability(const FIntPoint& Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const
{
	UpdateProbabilityAtPos(FIntPoint(Pos.X+1, Pos.Y), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos.X-1, Pos.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos.X, Pos.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos.X+1, Pos.Y+1), TileType, Probability, Probabilities);
}

/**
 * Metodo privado que actualiza el valore de la probabilidad de aparicion de un tipo de casilla en una posicion
 * concreta del Array2D
 * 
 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @param TileType Tipo de casilla a modificar
 * @param Probability Variacion en el valor de la probabilidad
 * @param Probabilities Array de probabilidades
 */
void ATileMap::UpdateProbabilityAtPos(const FIntPoint &Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability> &Probabilities) const
{
	// Se verifica que la posicion dada sea valida y no se salga de las dimensiones del mapa
	if (Pos.X-1 >= 0 && Pos.X+1 < Rows && Pos.Y+1 < Cols)
	{
		// Se obtiene la posicion en el Array1D y se actualiza segun el tipo de casilla a modificar
		const int32 UpdatePos = GetPositionInArray(Pos);
		switch (TileType)
		{
			case ETileType::Plains: Probabilities[UpdatePos].PlainsProbability += Probability; break;
			case ETileType::Hills: Probabilities[UpdatePos].HillsProbability += Probability; break;
			case ETileType::Forest: Probabilities[UpdatePos].ForestProbability += Probability; break;
			case ETileType::Snow: Probabilities[UpdatePos].SnowProbability += Probability; break;
			case ETileType::Ice: Probabilities[UpdatePos].IceProbability += Probability; break;
			case ETileType::Mountains: Probabilities[UpdatePos].MountainsProbability += Probability; break;
			case ETileType::Water: Probabilities[UpdatePos].WaterProbability += Probability; break;
			default: Probabilities[UpdatePos].Error++; break;
		}
	}
}

/**
 * Metodo privado que calcula el tipo de casilla a generar en el mapa
 * 
 * @param Pos1D Posicion en el Array1D
 * @param Pos2D Coordenadas en el Array2D
 * @param Probabilities Array de probabilidades de aparicion de los diferentes tipos de casillas
 * @return Tipo de casilla a generar
 */
TSubclassOf<ATile> ATileMap::GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FSTileProbability>& Probabilities) const
{
	TSubclassOf<ATile> GeneratedTile;
	if (Probabilities[Pos1D].IceProbability > 0.0)
	{
		if (FMath::RandRange(0.f, 1.f) <= Probabilities[Pos1D].IceProbability)
		{
			GeneratedTile = IceTile;
			// UpdateProbability(Pos2D, ETileType::Snow, 0.05, Probabilities);
		}
		else
		{
			const float RandVal = FMath::RandRange(0.f, 1.f);
			if (RandVal > Probabilities[Pos1D].WaterProbability-Probabilities[Pos1D].SnowProbability)
			{
				GeneratedTile = SnowTile;
			}
			else
			{
				GeneratedTile = WaterTile;
				UpdateProbability(Pos2D, ETileType::Hills, -0.1, Probabilities);
				UpdateProbability(Pos2D, ETileType::Water, WaterProbabilityModifier, Probabilities);
			}
		}
	}
	else if (FMath::RandRange(0.f, 1.0f) <= Probabilities[Pos1D].WaterProbability)
	{
		GeneratedTile = WaterTile;
		UpdateProbability(Pos2D, ETileType::Mountains, -0.2, Probabilities);
		UpdateProbability(Pos2D, ETileType::Water, WaterProbabilityModifier, Probabilities);
	}
	else
	{
		UpdateProbability(Pos2D, ETileType::Water, -0.1, Probabilities);
		
		const float TotalProbability =
			Probabilities[Pos1D].PlainsProbability + Probabilities[Pos1D].HillsProbability +
			Probabilities[Pos1D].ForestProbability + Probabilities[Pos1D].MountainsProbability;
		const float RandVal = FMath::RandRange(0.f, TotalProbability);
		
		float PrevAccumProbability = 0.0;
		float AccumProbability = Probabilities[Pos1D].PlainsProbability;
		if (PrevAccumProbability <= RandVal && RandVal <= AccumProbability)
		{
			if (Pos2D.X < NumIceRows+NumSnowRows || (Pos2D.X >= Rows-NumIceRows-NumSnowRows && Pos2D.X <= Rows-NumIceRows))
			{
				GeneratedTile = SnowTile;
			}
			else GeneratedTile = PlainsTile;
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].HillsProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			GeneratedTile = HillsTile;
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].ForestProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			GeneratedTile = ForestTile;
			UpdateProbability(Pos2D, ETileType::Forest, 0.1, Probabilities);
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].MountainsProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			GeneratedTile = MountainsTile;
			UpdateProbability(Pos2D, ETileType::Mountains, 0.1, Probabilities);
		}
	}
	
	return GeneratedTile;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void ATileMap::BeginPlay()
{
	Super::BeginPlay();

	NumIceRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 20.0));
	NumSnowRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 10.0));
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("NumIceRows - %d"), NumIceRows))
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("NumSnowRows - %d"), NumSnowRows))
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("NumColdRows - %d"), NumIceRows+NumSnowRows))

	switch (MapSeaLevel)
	{
		case EMapSeaLevel::Arid: WaterProbabilityModifier = 0.1; break;
		case EMapSeaLevel::Standard: WaterProbabilityModifier = 0.13; break;
		case EMapSeaLevel::Wet: WaterProbabilityModifier = 0.17; break;
	}
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("WaterProbabilityModifier - %f"), WaterProbabilityModifier))
	
	const int32 Dimension = Rows*Cols;
	TArray<FSTileProbability> Probabilities;
	
	Tiles.SetNumZeroed(Dimension);
	Probabilities.SetNumZeroed(Dimension);

	// UE_LOG(LogTemp, Log, TEXT("NumSteps %s"), *FString::Printf(TEXT("NumSteps - %d"), NumSteps))
	for (int32 Pos = 0, IceRow = -1; Pos < Dimension; ++Pos)
	{
		Probabilities[Pos].IceProbability = ProbabilityOfIce(Pos, IceRow);

		Probabilities[Pos].PlainsProbability = 0.15;
		Probabilities[Pos].HillsProbability = 0.15;
		Probabilities[Pos].ForestProbability = 0.15;
		Probabilities[Pos].MountainsProbability = 0.15;
		
		Probabilities[Pos].WaterProbability = WaterTileChance;
	}

	/*
	FString Cadena = TEXT("");
	for (int i = 1; i <= Probabilities.Num(); ++i)
	{
		Cadena += FString::Printf(TEXT("%f "), Probabilities[i-1].IceProbability);
		if (i%Rows == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *Cadena);
			Cadena = TEXT("");
		}
	}
	*/
	
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Cols; ++Col)
		{
			const float RowPos = Col * HorizontalOffset;
			const float ColPos = Col % 2 == 0 ? Row * VerticalOffset : Row * VerticalOffset + RowOffset;

			const int32 PositionInArray = GetPositionInArray(Row, Col);
			const TSubclassOf<ATile> TileToSpawn = GenerateTileType(PositionInArray, FIntPoint(Row, Col), Probabilities);

			ATile *NewTile = GetWorld()->SpawnActor<ATile>(TileToSpawn, FVector(FIntPoint(RowPos, ColPos)), FRotator::ZeroRotator);
			NewTile->SetPosition(FIntPoint(Row, Col));
			NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), Row, Col));

			Tiles[PositionInArray] = NewTile;
		}
	}
}

// Called every frame
// void ATileMap::Tick(float DeltaTime)
// {
//	Super::Tick(DeltaTime);

// }


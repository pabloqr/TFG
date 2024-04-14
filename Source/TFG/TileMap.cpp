// Fill out your copyright notice in the Description page of Project Settings.

#include "TileMap.h"

#include "JsonManager.h"
#include "Tile.h"

/**
 * Constructor de la clase que inicializa los parametros del actor
 */
ATileMap::ATileMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	Rows = 4;
	Cols = 12;

	RowOffset = 0.0;
	HorizontalOffset = 0.0;
	VerticalOffset = 0.0;

	WaterProbabilityModifier = 0.0;
	MapTemperature = EMapTemperature::Temperate;
	MapSeaLevel = EMapSeaLevel::Standard;

	NumIceRows = 0;
	NumSnowRows = 0;
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
float ATileMap::ProbabilityOfIce(const int32 Pos, int32& IceRow) const
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
 * circundantes a la actual. Para ello, llama al metodo UpdateProbabilityAtPos para las casillas correspondientes
 *
 * Dado que la generacion de casillas se realiza de forma iterativa, no tiene sentido que se modifique la probabilidad
 * para las casillas que ya han sido procesadas, por lo que tan solo se modificaran:
 *		- La casilla en la misma fila y en la siguiente columna
 *		- La casilla en la siguiente fila y en la columna anterior
 *		- La casilla en la siguiente fila y en la misma columna
 *		- La casilla en la siguiente fila y en la siguiente columna
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
 * concreta del Array2D. Para ello, verifica que la posicion sea valida para el mapa actual
 * 
 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @param TileType Tipo de casilla a modificar
 * @param Probability Variacion en el valor de la probabilidad
 * @param Probabilities Array de probabilidades
 */
void ATileMap::UpdateProbabilityAtPos(const FIntPoint& Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const
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
			case ETileType::SnowPlains: Probabilities[UpdatePos].SnowProbability += Probability; break;
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
 * El algoritmo tendrá en cuenta las posiciones de los polos para generar las casillas mas frias (Hielo, Nieve)
 * En funcion del tipo de casilla que aparezca, se modificara la probabilidad de aparicion de otros tipos para que
 * el mapa sea mas 'realista':
 *		- Agua:
 *			+ probabilidad para Agua (formacion de oceanos)
 *			- probabilidad para Montana
 *		- Montana:
 *			+ probabilidad para Montana (formacion de cordilleras)
 *		- Bosque:
 *			+ probabilidad para Bosque (formacion de bosques extensos)
 * 
 * @param Pos1D Posicion en el Array1D
 * @param Pos2D Coordenadas en el Array2D
 * @param Probabilities Array de probabilidades de aparicion de los diferentes tipos de casillas
 * @return Tipo de casilla a generar
 */
ETileType ATileMap::GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FSTileProbability>& Probabilities) const
{
	ETileType GeneratedTile = ETileType::None;

	// Primero se comprueba si estamos en una fila en la que puede aparecer Hielo (prob > 0)
	if (Probabilities[Pos1D].IceProbability > 0.0)
	{
		// Se genera la casilla, si no es de tipo Hielo sera de tipo Agua o Nieve
		if (FMath::RandRange(0.f, 1.f) <= Probabilities[Pos1D].IceProbability)
		{
			GeneratedTile = ETileType::Ice;
		}
		else
		{
			const float RandVal = FMath::RandRange(0.f, 1.f);
			if (RandVal > Probabilities[Pos1D].WaterProbability-Probabilities[Pos1D].SnowProbability)
			{
				GeneratedTile = ETileType::SnowPlains;
			}
			else
			{
				GeneratedTile = ETileType::Water;
				UpdateProbability(Pos2D, ETileType::Mountains, -0.1, Probabilities);
				UpdateProbability(Pos2D, ETileType::Water, WaterProbabilityModifier, Probabilities);
			}
		}
	}
	// Si no es una zona en la que puede aparecer Hielo, se generara una casilla de tipo Agua o una terrestre
	// (Llanura, Colinas, Bosque, Montana)
	else if (FMath::RandRange(0.f, 1.0f) <= Probabilities[Pos1D].WaterProbability)
	{
		GeneratedTile = ETileType::Water;
		UpdateProbability(Pos2D, ETileType::Mountains, -0.2, Probabilities);
		UpdateProbability(Pos2D, ETileType::Water, WaterProbabilityModifier, Probabilities);
	}
	else
	{
		UpdateProbability(Pos2D, ETileType::Water, -0.1, Probabilities);

		// Se tienen en cuenta todas las probabilidades por lo que se suman todos los valores y se genera un
		// valor aleatorio hasta ese numero, despues se comprueba por rangos para que se tenga en cuenta la
		// probabilidad correcta para cada tipo de casilla
		const float TotalProbability =
			Probabilities[Pos1D].PlainsProbability + Probabilities[Pos1D].HillsProbability +
			Probabilities[Pos1D].ForestProbability + Probabilities[Pos1D].MountainsProbability;
		const float RandVal = FMath::RandRange(0.f, TotalProbability);
		
		float PrevAccumProbability = 0.0;
		float AccumProbability = Probabilities[Pos1D].PlainsProbability;
		if (PrevAccumProbability <= RandVal && RandVal <= AccumProbability)
		{
			// Se debe comprobar si la casilla en la que aparece es fria o no para que aparezca Nieve o Llanura
			if (Pos2D.X < NumIceRows+NumSnowRows || (Pos2D.X >= Rows-NumIceRows-NumSnowRows && Pos2D.X <= Rows-NumIceRows))
			{
				GeneratedTile = ETileType::SnowPlains;
			}
			else GeneratedTile = ETileType::Plains;
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].HillsProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			// Se debe comprobar si la casilla en la que aparece es fria o no para que aparezca ColinasNevadas o Colinas
			if (Pos2D.X < NumIceRows+NumSnowRows || (Pos2D.X >= Rows-NumIceRows-NumSnowRows && Pos2D.X <= Rows-NumIceRows))
			{
				GeneratedTile = ETileType::SnowHills;
			}
			else GeneratedTile = ETileType::Hills;
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].ForestProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			GeneratedTile = ETileType::Forest;
			UpdateProbability(Pos2D, ETileType::Forest, 0.1, Probabilities);
		}

		PrevAccumProbability = AccumProbability;
		AccumProbability += Probabilities[Pos1D].MountainsProbability;
		if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
		{
			GeneratedTile = ETileType::Mountains;
			UpdateProbability(Pos2D, ETileType::Mountains, 0.1, Probabilities);
		}
	}
	
	return GeneratedTile;
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
 *
 * Inicializara los valores de los parametros teniendo en cuenta los que ya han sido actualizados por el
 * Blueprint y generara el mapa, almacenando los datos necesarios para su posterior procesamiento durante
 * la partida
 */
void ATileMap::BeginPlay()
{
	Super::BeginPlay();

	// Se inicializan los valores para el numero de filas que pueden contener hielo y casillas de nieve
	NumIceRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 20.0));
	NumSnowRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 10.0));

	// Se establece el modificador de probabilidad de aparicion de agua en funcion del tipo de mapa
	switch (MapSeaLevel)
	{
		case EMapSeaLevel::Arid: WaterProbabilityModifier = 0.1; break;
		case EMapSeaLevel::Standard: WaterProbabilityModifier = 0.13; break;
		case EMapSeaLevel::Wet: WaterProbabilityModifier = 0.17; break;
	}

	// Se declaran e inicializan los vectores necesarios para la generacion del mapa
	const int32 Dimension = Rows*Cols;
	TArray<FSTileProbability> Probabilities;
	
	Tiles.SetNumZeroed(Dimension);
	Probabilities.SetNumZeroed(Dimension);

	// Se inicializa el array de probabilidaddes con los valores calculados o por defecto en funcion del tipo de casilla
	for (int32 Pos = 0, IceRow = -1; Pos < Dimension; ++Pos)
	{
		Probabilities[Pos].IceProbability = ProbabilityOfIce(Pos, IceRow);

		Probabilities[Pos].PlainsProbability = 0.15;
		Probabilities[Pos].HillsProbability = 0.15;
		Probabilities[Pos].ForestProbability = 0.15;
		Probabilities[Pos].MountainsProbability = 0.15;
		
		Probabilities[Pos].WaterProbability = WaterTileChance;
	}

	// Se genera el mapa
	// Se recorren primero las filas y, despues, las columnas de forma que se establece su posicion en el mapa,
	// se calcula la nueva casilla a generar, se genera en el juego y se actualiza el array de casillas interno
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Cols; ++Col)
		{
			const float RowPos = Col * HorizontalOffset;
			const float ColPos = Col % 2 == 0 ? Row * VerticalOffset : Row * VerticalOffset + RowOffset;

			const int32 PositionInArray = GetPositionInArray(Row, Col);
			const ETileType TileType = GenerateTileType(PositionInArray, FIntPoint(Row, Col), Probabilities);

			TSubclassOf<ATile> TileToSpawn;
			switch (TileType)
			{
				case ETileType::Plains: TileToSpawn = PlainsTile; break;
				case ETileType::Hills: TileToSpawn = HillsTile; break;
				case ETileType::Forest: TileToSpawn = ForestTile; break;
				case ETileType::SnowPlains: TileToSpawn = SnowTile; break;
				case ETileType::SnowHills: TileToSpawn = HillsTile; break;
				case ETileType::Ice: TileToSpawn = IceTile; break;
				case ETileType::Mountains: TileToSpawn = MountainsTile; break;
				case ETileType::Water: TileToSpawn = WaterTile; break;
				default: TileToSpawn = nullptr; break;
			}

			ATile *NewTile = GetWorld()->SpawnActor<ATile>(TileToSpawn, FVector(FIntPoint(RowPos, ColPos)), FRotator::ZeroRotator);
			NewTile->SetPosition(FIntPoint(Row, Col));
			NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), Row, Col));

			Tiles[PositionInArray] = NewTile;
		}
	}

	// Conversion a Json y guardado
	MapToJson();
}

//--------------------------------------------------------------------------------------------------------------------//

void ATileMap::MapToJson()
{
	TArray<FJsonMapDataEntry> StructArray;
	StructArray.SetNumZeroed(Tiles.Num());

	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		const ATile *Tile = Tiles[i];

		int32 TileType;
		switch (Tile->GetTileType())
		{
			case ETileType::Plains: TileType = 0; break;
			case ETileType::Hills: TileType = 0; break;
			case ETileType::Forest: TileType = 0; break;
			case ETileType::SnowPlains: TileType = 0; break;
			case ETileType::SnowHills: TileType = 0; break;
			case ETileType::Ice: TileType = 0; break;
			case ETileType::Mountains: TileType = 0; break;
			case ETileType::Water: TileType = 0; break;
			default: TileType = -1; break;
		}
		
		FJsonMapDataEntry JsonEntry = FJsonMapDataEntry(Tile->GetMapPosition().X, Tile->GetMapPosition().Y, TileType);
		StructArray.Add(JsonEntry);
	}

	FJsonMapData JsonData;
	for (const FJsonMapDataEntry &Entry : StructArray)
	{
		const TSharedPtr<FJsonObject> JsonEntry = MakeShareable(new FJsonObject);
		JsonEntry->SetNumberField("Row", Entry.Row);
		JsonEntry->SetNumberField("Col", Entry.Col);
		JsonEntry->SetNumberField("TileType", Entry.TileType);

		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("Entry.Row - %d"), Entry.Row))
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("Entry.Col - %d"), Entry.Col))
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("Entry.TileType - %d"), Entry.TileType))

		JsonData.Tiles.Add(MakeShareable(new FJsonValueObject(JsonEntry)));

		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("JsonEntry.Row - %f"), JsonEntry->GetNumberField("Row")))
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("JsonEntry.Col - %f"), JsonEntry->GetNumberField("Col")))
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("JsonEntry.TileType - %f"), JsonEntry->GetNumberField("TileType")))
	}

	bool Success = true;
	FString ResultMessage = "";
	UJsonManager::MapStructToJson("C:/Users/Pablo/AppData/Local/Temp/Test.json", JsonData, Success, ResultMessage);
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("Success - %hhd"), Success))
	UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("ResultMessage - %s"), *ResultMessage))
}

void ATileMap::JsonToMap()
{
}

// Called every frame
// void ATileMap::Tick(float DeltaTime)
// {
//	Super::Tick(DeltaTime);

// }


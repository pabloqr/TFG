// Fill out your copyright notice in the Description page of Project Settings.

#include "TileMap.h"

#include "JsonManager.h"
#include "MapSave.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"

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

	WaterProbabilityModifier = 0.13;
	MapTemperature = EMapTemperature::Temperate;
	MapSeaLevel = EMapSeaLevel::Standard;

	NumIceRows = 0;
	NumSnowRows = 0;
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo que calcula la probabilidad de que una casilla sea Hielo (IceTile), se hara para que se acumule
 * en los polos
 * 
 * @param Pos1D Posicion en el Array1D
 * @param IceRow Indice dentro del numero de filas que pueden contener Hielo
 * @return Probabilidad de que la casilla en la posicion dada pueda contener Hielo
 */
float ATileMap::ProbabilityOfIce(const int32 Pos1D, int32& IceRow) const
{
	float CurrentProbability = 0.8;
	const int32 Row = GetRowInMap(Pos1D);
	const int32 Col = GetColInMap(Pos1D);

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
 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @param TileType Tipo de casilla a modificar
 * @param Probability Variacion en el valor de la probabilidad
 * @param Probabilities Array de probabilidades
 */
void ATileMap::UpdateProbability(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const
{
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X+1, Pos2D.Y), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X-1, Pos2D.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X, Pos2D.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X+1, Pos2D.Y+1), TileType, Probability, Probabilities);
}

/**
 * Metodo privado que actualiza el valore de la probabilidad de aparicion de un tipo de casilla en una posicion
 * concreta del Array2D. Para ello, verifica que la posicion sea valida para el mapa actual
 * 
 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 * @param TileType Tipo de casilla a modificar
 * @param Probability Variacion en el valor de la probabilidad
 * @param Probabilities Array de probabilidades
 */
void ATileMap::UpdateProbabilityAtPos(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const
{
	// Se verifica que la posicion dada sea valida y no se salga de las dimensiones del mapa
	if (Pos2D.X-1 >= 0 && Pos2D.X+1 < Rows && Pos2D.Y+1 < Cols)
	{
		// Se obtiene la posicion en el Array1D y se actualiza segun el tipo de casilla a modificar
		const int32 UpdatePos = GetPositionInArray(Pos2D);
		switch (TileType)
		{
			case ETileType::Plains: Probabilities[UpdatePos].PlainsProbability += CheckProbability(Probabilities[UpdatePos].PlainsProbability, Probability); break;
			case ETileType::Hills: Probabilities[UpdatePos].HillsProbability += CheckProbability(Probabilities[UpdatePos].HillsProbability, Probability); break;
			case ETileType::Forest: Probabilities[UpdatePos].ForestProbability += CheckProbability(Probabilities[UpdatePos].ForestProbability, Probability); break;
			case ETileType::Ice: Probabilities[UpdatePos].IceProbability += CheckProbability(Probabilities[UpdatePos].IceProbability, Probability); break;
			case ETileType::Mountains: Probabilities[UpdatePos].MountainsProbability += CheckProbability(Probabilities[UpdatePos].MountainsProbability, Probability); break;
			case ETileType::Water: Probabilities[UpdatePos].WaterProbability += CheckProbability(Probabilities[UpdatePos].WaterProbability, Probability); break;
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
			if (RandVal > Probabilities[Pos1D].WaterProbability)
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

TSubclassOf<ATile> ATileMap::SelectTileType(const ETileType TileType) const
{
	TSubclassOf<ATile> SelectedTile;
	switch (TileType)
	{
		case ETileType::Plains: SelectedTile = PlainsTile; break;
		case ETileType::Hills: SelectedTile = HillsTile; break;
		case ETileType::Forest: SelectedTile = ForestTile; break;
		case ETileType::SnowPlains: SelectedTile = SnowTile; break;
		case ETileType::SnowHills: SelectedTile = HillsTile; break;
		case ETileType::Ice: SelectedTile = IceTile; break;
		case ETileType::Mountains: SelectedTile = MountainsTile; break;
		case ETileType::Water: SelectedTile = WaterTile; break;
		default: SelectedTile = nullptr; break;
	}

	return SelectedTile;
}

void ATileMap::SetTileAtPos(const int32 Pos1D, const FIntPoint& Pos2D, const ETileType TileType)
{
	const float RowPos = Pos2D.Y * HorizontalOffset;
	const float ColPos = Pos2D.Y % 2 == 0 ? Pos2D.X * VerticalOffset : Pos2D.X * VerticalOffset + RowOffset;

	const TSubclassOf<ATile> TileToSpawn = SelectTileType(TileType);
	
	ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileToSpawn, FVector(FIntPoint(RowPos, ColPos)), FRotator::ZeroRotator);
	// UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("[%d][%d] valid: %d"), Pos2D.X, Pos2D.Y, NewTile == nullptr))
	if (NewTile != nullptr)
	{
		NewTile->SetPosition(FIntPoint(Pos2D.X, Pos2D.Y));
		// TODO quitar para lanzamiento
		NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), Pos2D.X, Pos2D.Y));
	}

	Tiles[Pos1D] = NewTile;
}

void ATileMap::SetMapFromSave(const TArray<FMapData>& TilesInfo)
{
	Tiles.SetNumZeroed(TilesInfo.Num());
	for (int32 i = 0; i < TilesInfo.Num(); ++i)
	{
		// UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d, %d, %d)"), TilesInfo[i].Row, TilesInfo[i].Col, TilesInfo[i].TileType))

		ATile* Tile = Tiles[i];
		const ETileType TileType = ATile::IntToTileType(TilesInfo[i].TileType);
		
		if (Tile->GetTileType() != TileType)
		{
			const FIntPoint TilePos = Tile->GetMapPosition();
			Tile->Destroy();

			SetTileAtPos(i, TilePos, TileType);
		}
	}
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
		Probabilities[Pos].MountainsProbability = 0.1;
		
		Probabilities[Pos].WaterProbability = WaterTileChance;
	}

	// Se genera el mapa
	// Se recorren primero las filas y, despues, las columnas de forma que se establece su posicion en el mapa,
	// se calcula la nueva casilla a generar, se genera en el juego y se actualiza el array de casillas interno
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Cols; ++Col)
		{
			const int32 Pos1D = GetPositionInArray(Row, Col);
			const FIntPoint Pos2D = FIntPoint(Row, Col);
			const ETileType TileType = GenerateTileType(Pos1D, Pos2D, Probabilities);

			SetTileAtPos(Pos1D, Pos2D, TileType);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo que almacena la informacion de las casillas en un archivo de guardado para su posterior carga
 */
void ATileMap::SaveMap() const
{
	TArray<FMapData> MapData;
	MapData.SetNumZeroed(Tiles.Num());
	
	if (UMapSave* SaveGameInstance = Cast<UMapSave>(UGameplayStatics::CreateSaveGameObject(UMapSave::StaticClass())))
	{
		SaveGameInstance->TilesInfo = MapData;
		if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MapTest"), 0))
		{
			UE_LOG(LogTemp, Error, TEXT("ERROR: fallo al intentar guardar el mapa"))
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Guardado correcto"))
	}
}

/**
 * Metodo que lee la informacion de las casillas de un archivo de guardado para actualizar el mapa
 */
void ATileMap::LoadMap()
{
	if (const UMapSave* LoadedGame = Cast<UMapSave>(UGameplayStatics::LoadGameFromSlot(TEXT("MapTest"), 0)))
	{
		SetMapFromSave(LoadedGame->TilesInfo);
	}
}

/**
 * Metodo que transforma la informacion de las casillas para que pueda ser almacenada en un archivo Json
 */
void ATileMap::MapToJson()
{
	TArray<FMapData> JsonData;
	JsonData.SetNumZeroed(Tiles.Num());

	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		const ATile *Tile = Tiles[i];
		const int32 TileType = ATile::TileTypeToInt(Tile->GetTileType());
		
		JsonData[i] = FMapData(Tile->GetMapPosition().X, Tile->GetMapPosition().Y, TileType);
	}

	bool Success = true;
	FString ResultMessage = "";
	UJsonManager::MapStructToJson("C:/Users/Pablo/AppData/Local/Temp/Test.json", JsonData, Success, ResultMessage);
}

/**
 * Metodo que transforma la informacion sobre las casillas de un archivo Json para actualizar el mapa
 */
void ATileMap::JsonToMap()
{
	bool Success = true;
	FString ResultMessage = "";
	const TArray<FMapData> JsonData = UJsonManager::JsonToMapStruct("C:/Users/Pablo/AppData/Local/Temp/Test.json", Success, ResultMessage);

	if (Success) SetMapFromSave(JsonData);
	else UE_LOG(LogTemp, Error, TEXT("%s"), *ResultMessage)
}

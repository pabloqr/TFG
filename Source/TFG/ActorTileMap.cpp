#include "ActorTileMap.h"

#include "SaveMap.h"
#include "GInstance.h"
#include "Kismet/GameplayStatics.h"

AActorTileMap::AActorTileMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridSize = FVector2D(0.0, 0.0);

	Rows = 32;
	Cols = 64;

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

float AActorTileMap::ProbabilityOfIce(const int32 Pos1D, int32& IceRow) const
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

void AActorTileMap::UpdateProbability(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FTileProbability>& Probabilities) const
{
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X+1, Pos2D.Y), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X-1, Pos2D.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X, Pos2D.Y+1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X+1, Pos2D.Y+1), TileType, Probability, Probabilities);
}

void AActorTileMap::UpdateProbabilityAtPos(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FTileProbability>& Probabilities) const
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

ETileType AActorTileMap::GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FTileProbability>& Probabilities) const
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

void AActorTileMap::SetTileAtPos(const int32 Pos1D, const FIntPoint& Pos2D, const ETileType TileType)
{
	GridSize.X = Pos2D.Y * HorizontalOffset;
	GridSize.Y = Pos2D.Y % 2 == 0 ? Pos2D.X * VerticalOffset : Pos2D.X * VerticalOffset + RowOffset;

	TilesInfo[Pos1D] = FTileInfo(Pos2D, GridSize, TileType);
}

void AActorTileMap::SetMapFromSave(const TArray<FMapData>& TilesData)
{
	Tiles.SetNumZeroed(TilesData.Num());
	for (int32 i = 0; i < TilesData.Num(); ++i)
	{
		AActorTile* Tile = Tiles[i];
		const ETileType TileType = AActorTile::IntToTileType(TilesData[i].TileType);
		
		if (Tile->GetType() != TileType)
		{
			const FIntPoint TilePos = Tile->GetPos();
			Tile->Destroy();

			SetTileAtPos(i, TilePos, TileType);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorTileMap::IsTileAccesible(const FIntPoint& Pos2D) const
{
	return CheckValidPosition(Pos2D, FIntPoint(Rows, Cols)) && Tiles[GetPositionInArray(Pos2D)]->IsAccesible();
}

AActorTile* AActorTileMap::GetTileAtPos(const FIntPoint& Pos2D) const
{
	return CheckValidPosition(Pos2D, FIntPoint(Rows, Cols)) ? Tiles[GetPositionInArray(Pos2D)] : nullptr;
}

TArray<FMovement> AActorTileMap::FindPath(const FIntPoint& PosIni, const FIntPoint& PosEnd)
{
	// Limite del mapa
	const FIntPoint Limit = FIntPoint(Rows, Cols);

	// Se comprueba que los datos son correctos, si no lo son, se devuelve un array vacio
	if (!(CheckValidPosition(PosIni, Limit) && CheckValidPosition(PosIni, Limit)) ||
		!(Tiles[GetPositionInArray(PosIni)]->IsAccesible() && Tiles[GetPositionInArray(PosEnd)]->IsAccesible()))
	{
		return TArray<FMovement>();
	}
	
	// Se crea una lista con prioridad para almacenar los nodos por visitar ordenados de mayor a menor prioridad
	// teniendo en cuenta que la prioridad se basa en la cercania al objetivo y el coste
	//
	// Inicialmente, se inserta el nodo inicial
	FPriorityQueue Frontier;
	Frontier.Push(FPathData(PosIni, 0));

	// Se crea un diccionario que almacena, para cada nodo, desde cual se ha llegado a el
	TMap<FIntPoint, FIntPoint> CameFrom;
	CameFrom.Add(PosIni, FIntPoint(-1, -1));
	// Se crea un diccionario que almacena, para cada nodo, el coste de llegar a el
	TMap<FIntPoint, int32> TotalCost;
	TotalCost.Add(PosIni, 0);

	// Se crea el almacen del camino a seguir
	TArray<FMovement> Path;

	// Se procesan nodos mientras sigan quedando
	while (!Frontier.Empty())
	{
		// Se obtiene el nodo con la mayor prioridad y se comprueba si se ha llegado al destino
		const FPathData CurrentData = Frontier.Pop();
		if (CurrentData.Pos2D == PosEnd)
		{
			// Se procesan todos los nodos del diccionario CameFrom que nos permite conocer el camino de vuelta
			// a la posicion inicial desde el objetivo
			FIntPoint Current = CurrentData.Pos2D;
			while (Current != PosIni)
			{
				// Se anade el nodo actual al camino a devolver y se actualiza al siguiente nodo
				Path.Insert(FMovement(Current, Tiles[GetPositionInArray(Current)]->GetMovementCost()), 0);
				Current = CameFrom[Current];
			}
			
			break;
		}

		// Se obtiene la casilla actual dada su posicion en el Array2D, se calculan los vecinos de la casilla actual
		// y se procesan
		const AActorTile* CurrentTile = Tiles[GetPositionInArray(CurrentData.Pos2D)];
		for (const FIntPoint NeighborPos : CurrentTile->GetNeighbors())
		{
			// Se verifica si el vecino calculado es correcto y, en caso de ser accesible, se obtiene
			const AActorTile* NeighborTile;
			if (CheckValidPosition(NeighborPos, Limit) && (NeighborTile = Tiles[GetPositionInArray(NeighborPos)])->IsAccesible())
			{
				// Se calcula el coste de llegar a esta casilla junto con el coste de movimiento de la propia casilla
				int32 NewCost = TotalCost[CurrentData.Pos2D] + NeighborTile->GetMovementCost();
				if (!TotalCost.Contains(NeighborPos))
				{
					// Si el nodo no se habia procesado previamente, se anade a las diferentes estructuras
					// con los valores de prioridad y coste correctos
					TotalCost.Add(NeighborPos, NewCost);

					const int32 Priority = NewCost + NeighborTile->GetDistanceToElement(PosEnd);
					Frontier.Push(FPathData(NeighborPos, Priority));
					
					CameFrom.Add(NeighborPos, CurrentData.Pos2D);
				}
				else
				{
					// Si el nodo se habia procesado previamente, se obtiene una referencia a su coste y, en caso
					// de ser mayor al nuevo coste calculado, se actualizan todos los valores de las diferentes
					// estructuras
					int32& CurrentCost = TotalCost[NeighborPos];
					if (NewCost < CurrentCost)
					{
						CurrentCost = NewCost;
						
						const int32 Priority = NewCost + NeighborTile->GetDistanceToElement(PosEnd);
						Frontier.Push(FPathData(NeighborPos, Priority));

						CameFrom[NeighborPos] = CurrentData.Pos2D;
					}
				}
			}
		}
	}

	return Path;
}

void AActorTileMap::GenerateMap(const EMapTemperature MapTemp, const EMapSeaLevel MapSeaLvl)
{
	// Se actualizan los valores para los modificadores del mapa
	MapTemperature = MapTemp;
	MapSeaLevel = MapSeaLvl;
	
	// Se inicializan los valores para el numero de filas que pueden contener hielo y casillas de nieve
	NumIceRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 20.0));
	NumSnowRows = FMath::Max(1, static_cast<int32>(Rows * static_cast<uint8>(MapTemperature) / 12.0));

	// Se establece el modificador de probabilidad de aparicion de agua en funcion del tipo de mapa
	switch (MapSeaLevel)
	{
		case EMapSeaLevel::Arid: WaterProbabilityModifier = 0.1; break;
		case EMapSeaLevel::Standard: WaterProbabilityModifier = 0.13; break;
		case EMapSeaLevel::Wet: WaterProbabilityModifier = 0.17; break;
	}

	// Se declaran e inicializan los vectores necesarios para la generacion del mapa
	const int32 Dimension = Rows*Cols;
	TArray<FTileProbability> Probabilities;

	TilesInfo.SetNumZeroed(Dimension);
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

	UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		GameInstance->GridSize = FVector2D(GridSize.X, GridSize.Y - RowOffset);
	}
}

void AActorTileMap::DisplayTileAtPos(const TSubclassOf<AActorTile> Tile, const FTileInfo& TileInfo)
{
	// Se calcula la rotacion de la casilla que se va a anadir a la escena
	const int32 RotationSelector = FMath::RandRange(0, 5);
	const float Rotation = TileInfo.TileType == ETileType::Mountains ? 180.0 / 3.0 * RotationSelector : 0.0;

	// Se anade la casilla a la escena
	AActorTile* NewTile = GetWorld()->SpawnActor<AActorTile>(
		Tile,
		FVector(TileInfo.MapPos2D.X, TileInfo.MapPos2D.Y, 0.0),
		FRotator(0, Rotation, 0));

	// Se actualizan los atributos de la casilla
	if (NewTile != nullptr)
	{
		NewTile->SetPos(TileInfo.Pos2D, TileInfo.MapPos2D);
		NewTile->SetType(TileInfo.TileType);
		
		// TODO quitar para lanzamiento
		NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), TileInfo.Pos2D.X, TileInfo.Pos2D.Y));
	}

	// Se actualiza el array de casillas con la que se ha anadido
	Tiles[GetPositionInArray(TileInfo.Pos2D)] = NewTile;
}

void AActorTileMap::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorTileMap::SaveMap() const
{
	TArray<FMapData> MapData;
	MapData.SetNumZeroed(Tiles.Num());
	
	if (USaveMap* SaveGameInstance = Cast<USaveMap>(UGameplayStatics::CreateSaveGameObject(USaveMap::StaticClass())))
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

void AActorTileMap::LoadMap()
{
	if (const USaveMap* LoadedGame = Cast<USaveMap>(UGameplayStatics::LoadGameFromSlot(TEXT("MapTest"), 0)))
	{
		SetMapFromSave(LoadedGame->TilesInfo);
	}
}

/*
void AActorTileMap::MapToJson()
{
	TArray<FMapDataForJson> JsonData;
	JsonData.SetNumZeroed(Tiles.Num());

	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		const AActorTile *Tile = Tiles[i];
		const int32 TileType = AActorTile::TileTypeToInt(Tile->GetType());
		
		JsonData[i] = FMapDataForJson(Tile->GetMapPosition().X, Tile->GetMapPosition().Y, TileType);
	}

	bool Success = true;
	FString ResultMessage = "";
	UJsonManager::MapStructToJson("C:/Users/Pablo/AppData/Local/Temp/Test.json", JsonData, Success, ResultMessage);
}

void AActorTileMap::JsonToMap()
{
	bool Success = true;
	FString ResultMessage = "";
	const TArray<FMapDataForJson> JsonData = UJsonManager::JsonToMapStruct("C:/Users/Pablo/AppData/Local/Temp/Test.json", Success, ResultMessage);

	if (Success) SetMapFromSave(JsonData);
	else UE_LOG(LogTemp, Error, TEXT("%s"), *ResultMessage)
}
*/

//--------------------------------------------------------------------------------------------------------------------//

void AActorTileMap::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

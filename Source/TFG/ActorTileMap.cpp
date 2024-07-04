#include "ActorTileMap.h"

#include "ActorCivilUnit.h"
#include "ActorDamageableElement.h"
#include "ActorResource.h"
#include "ActorSettlement.h"
#include "ActorTile.h"
#include "SaveMap.h"
#include "GInstance.h"
#include "LibraryTileMap.h"
#include "Kismet/GameplayStatics.h"

AActorTileMap::AActorTileMap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TilesInfo = TMap<FIntPoint, FTileInfo>();
	TilesWithState = TMap<ETileState, FTilesArray>();
	TileTypeCount = TMap<ETileType, int32>({
		{ETileType::Plains, 0},
		{ETileType::Hills, 0},
		{ETileType::Forest, 0},
		{ETileType::SnowPlains, 0},
		{ETileType::SnowHills, 0},
		{ETileType::Ice, 0},
		{ETileType::Mountains, 0},
		{ETileType::Water, 0}
	});
	ResourceCount = TMap<EResource, int32>({
		{EResource::Diamond, 0},
		{EResource::Gold, 0},
		{EResource::Copper, 0},
		{EResource::Aluminium, 0},
		{EResource::Coal, 0},
		{EResource::Oil, 0},
	});

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
		IceRow += Row <= NumIceRows ? 1 : Row >= Rows - NumIceRows ? -1 : 0;
	}
	// Si la casilla esta fuera del rango de casillas que pueden contener Hielo, la probabilidad es 0
	// En caso contrario, se verifica el numero de filas que pueden contener Hielo para aplicar ajustes
	if (IceRow >= NumIceRows) CurrentProbability = 0.0;
	else if (NumIceRows == 1)
	{
		// Si el numero de filas que pueden contener Hielo es 1, limitaremos la aparicion de este tipo de casillas
		// A aquellas que se situen en el limite exterior del mapa dado que el tamano del mapa es pequeno
		const bool CorrectPos = (Row < NumIceRows && Col % 2 != 0) || (Row >= Rows - NumIceRows && Col % 2 == 0);
		CurrentProbability = CorrectPos ? 0.0 : CurrentProbability;
	}

	for (int32 i = 0; i < IceRow && IceRow > 0 && IceRow < NumIceRows; ++i)
	{
		CurrentProbability *= FMath::Pow(0.1 / CurrentProbability, 1.f / NumIceRows);
	}

	return CurrentProbability;
}

void AActorTileMap::UpdateProbability(const FIntPoint& Pos2D, const ETileType TileType, const float Probability,
                                      TArray<FTileProbability>& Probabilities) const
{
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X + 1, Pos2D.Y), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X - 1, Pos2D.Y + 1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X, Pos2D.Y + 1), TileType, Probability, Probabilities);
	UpdateProbabilityAtPos(FIntPoint(Pos2D.X + 1, Pos2D.Y + 1), TileType, Probability, Probabilities);
}

void AActorTileMap::UpdateProbabilityAtPos(const FIntPoint& Pos2D, const ETileType TileType, const float Probability,
                                           TArray<FTileProbability>& Probabilities) const
{
	// Se verifica que la posicion dada sea valida y no se salga de las dimensiones del mapa
	if (Pos2D.X - 1 >= 0 && Pos2D.X + 1 < Rows && Pos2D.Y + 1 < Cols)
	{
		// Se obtiene y verifica la posicion en el Array1D y se actualiza segun el tipo de casilla a modificar
		const int32 UpdatePos = GetPositionInArray(Pos2D);
		if (UpdatePos != -1)
		{
			switch (TileType)
			{
			case ETileType::Plains: Probabilities[UpdatePos].PlainsProbability += CheckProbability(
					Probabilities[UpdatePos].PlainsProbability, Probability);
				break;
			case ETileType::Hills: Probabilities[UpdatePos].HillsProbability += CheckProbability(
					Probabilities[UpdatePos].HillsProbability, Probability);
				break;
			case ETileType::Forest: Probabilities[UpdatePos].ForestProbability += CheckProbability(
					Probabilities[UpdatePos].ForestProbability, Probability);
				break;
			case ETileType::Ice: Probabilities[UpdatePos].IceProbability += CheckProbability(
					Probabilities[UpdatePos].IceProbability, Probability);
				break;
			case ETileType::Mountains: Probabilities[UpdatePos].MountainsProbability += CheckProbability(
					Probabilities[UpdatePos].MountainsProbability, Probability);
				break;
			case ETileType::Water: Probabilities[UpdatePos].WaterProbability += CheckProbability(
					Probabilities[UpdatePos].WaterProbability, Probability);
				break;
			default: Probabilities[UpdatePos].Error++;
				break;
			}
		}
	}
}

ETileType AActorTileMap::GenerateTileType(const FIntPoint& Pos, TArray<FTileProbability>& Probabilities) const
{
	ETileType GeneratedTile = ETileType::None;

	// Se obtiene el indice de la casilla a generar y se verifica que sea correcto
	const int32 Index = GetPositionInArray(Pos);
	if (Index != -1)
	{
		// Primero se comprueba si estamos en una fila en la que puede aparecer Hielo (prob > 0)
		if (Probabilities[Index].IceProbability > 0.0)
		{
			// Se genera la casilla, si no es de tipo Hielo sera de tipo Agua o Nieve
			if (FMath::RandRange(0.f, 1.f) <= Probabilities[Index].IceProbability)
			{
				GeneratedTile = ETileType::Ice;
			}
			else
			{
				const float RandVal = FMath::RandRange(0.f, 1.f);
				if (RandVal > Probabilities[Index].WaterProbability)
				{
					GeneratedTile = ETileType::SnowPlains;
				}
				else
				{
					GeneratedTile = ETileType::Water;
					UpdateProbability(Pos, ETileType::Mountains, -0.1, Probabilities);
					UpdateProbability(Pos, ETileType::Water, WaterProbabilityModifier, Probabilities);
				}
			}
		}
		// Si no es una zona en la que puede aparecer Hielo, se generara una casilla de tipo Agua o una terrestre
		// (Llanura, Colinas, Bosque, Montana)
		else if (FMath::RandRange(0.f, 1.0f) <= Probabilities[Index].WaterProbability)
		{
			GeneratedTile = ETileType::Water;
			UpdateProbability(Pos, ETileType::Mountains, -0.2, Probabilities);
			UpdateProbability(Pos, ETileType::Water, WaterProbabilityModifier, Probabilities);
		}
		else
		{
			UpdateProbability(Pos, ETileType::Water, -0.1, Probabilities);

			// Se tienen en cuenta todas las probabilidades por lo que se suman todos los valores y se genera un
			// valor aleatorio hasta ese numero, despues se comprueba por rangos para que se tenga en cuenta la
			// probabilidad correcta para cada tipo de casilla
			const float TotalProbability =
				Probabilities[Index].PlainsProbability + Probabilities[Index].HillsProbability +
				Probabilities[Index].ForestProbability + Probabilities[Index].MountainsProbability;
			const float RandVal = FMath::RandRange(0.f, TotalProbability);

			float PrevAccumProbability = 0.0;
			float AccumProbability = Probabilities[Index].PlainsProbability;
			if (PrevAccumProbability <= RandVal && RandVal <= AccumProbability)
			{
				// Se debe comprobar si la casilla en la que aparece es fria o no para que aparezca Nieve o Llanura
				if (Pos.X < NumIceRows + NumSnowRows || (Pos.X >= Rows - NumIceRows - NumSnowRows && Pos.X <= Rows -
					NumIceRows))
				{
					GeneratedTile = ETileType::SnowPlains;
				}
				else GeneratedTile = ETileType::Plains;
			}

			PrevAccumProbability = AccumProbability;
			AccumProbability += Probabilities[Index].HillsProbability;
			if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
			{
				// Se debe comprobar si la casilla en la que aparece es fria o no para que aparezca ColinasNevadas o Colinas
				if (Pos.X < NumIceRows + NumSnowRows || (Pos.X >= Rows - NumIceRows - NumSnowRows && Pos.X <= Rows -
					NumIceRows))
				{
					GeneratedTile = ETileType::SnowHills;
				}
				else GeneratedTile = ETileType::Hills;
			}

			PrevAccumProbability = AccumProbability;
			AccumProbability += Probabilities[Index].ForestProbability;
			if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
			{
				GeneratedTile = ETileType::Forest;
				UpdateProbability(Pos, ETileType::Forest, 0.1, Probabilities);
			}

			PrevAccumProbability = AccumProbability;
			AccumProbability += Probabilities[Index].MountainsProbability;
			if (PrevAccumProbability < RandVal && RandVal <= AccumProbability)
			{
				GeneratedTile = ETileType::Mountains;
				UpdateProbability(Pos, ETileType::Mountains, 0.1, Probabilities);
			}
		}
	}

	return GeneratedTile;
}

void AActorTileMap::SetTileAtPos(const FIntPoint& Pos2D, const ETileType TileType)
{
	// Se actualiza la posicion en la escena de la casilla
	GridSize.X = Pos2D.Y * HorizontalOffset;
	GridSize.Y = Pos2D.Y % 2 == 0 ? Pos2D.X * VerticalOffset : Pos2D.X * VerticalOffset + RowOffset;

	// Se anade la informacion de la casilla al diccionario que la almacena
	TilesInfo.Add(Pos2D, FTileInfo(Pos2D, GridSize, TileType, FTileElements(), {ETileState::None}));
	// Se actualiza el diccionaro que almacena el conteo de casillas por tipo
	TileTypeCount[TileType] += 1;

	// Se llama al evento para que todos los suscriptores realicen las operaciones definidas
	OnTileInfoUpdated.Broadcast(Pos2D);
}

void AActorTileMap::SetMapFromSave(const TArray<FMapSaveData>& TilesData)
{
	// Se inicializa el array de casillas y se procesan todas las casillas del archivo de guardado
	Tiles.SetNumZeroed(TilesData.Num());
	for (int32 i = 0; i < TilesData.Num(); ++i)
	{
		const ETileType TileType = TilesData[i].Type;
		const FIntPoint Pos = TilesData[i].Pos2D;

		UpdateTileAtPos(Pos, TileType);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

int32 AActorTileMap::GetPositionInArray(const int32 Row, const int32 Col) const
{
	const bool IsValid = ULibraryTileMap::CheckValidPosition(FIntPoint(Row, Col), FIntPoint(Rows, Cols));

	int32 Index;
	return IsValid && (Index = Row * Cols + Col) < Tiles.Num() ? Index : -1;
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorTileMap::IsTileAccesible(const FIntPoint& Pos) const
{
	// Se verifica el indice, si no es correcto, se devuelve 'false'
	const int32 Index = GetPositionInArray(Pos);
	return Index != -1 ? Tiles[Index]->IsAccesible() : false;
}

AActorTile* AActorTileMap::GetTileAtPos(const FIntPoint& Pos) const
{
	// Se verifica el indice, si no es correcto, se devuelve 'nullptr'
	const int32 Index = GetPositionInArray(Pos);
	return Index != -1 ? Tiles[Index] : nullptr;
}

TArray<FIntPoint> AActorTileMap::GetTilesWithState(const ETileState& State) const
{
	return TilesWithState.Contains(State) ? TilesWithState[State].TilesArray : TArray<FIntPoint>();
}

TArray<FIntPoint> AActorTileMap::GetTilesWithinRange(const FIntPoint& Pos2D, const int32 Range)
{
	// Se inicializa la lista de casillas alcanzables
	TArray<FIntPoint> InRange = TArray<FIntPoint>();

	// Si no se tiene alcance, no se realiza ningun procesamiento
	if (Range > 0)
	{
		// Se obtienen los vecinos de la casilla actual y se procesan
		TArray<FIntPoint> Neighbors = ULibraryTileMap::GetNeighbors(Pos2D, FIntPoint(Rows, Cols));
		for (const FIntPoint Neighbor : Neighbors)
		{
			// Se obtiene el indice y se verifica que es valido, en caso contrario, se omite el vecino
			const int32 Index = GetPositionInArray(Neighbor);
			if (Index == -1) continue;

			// Se obtiene la casilla
			const AActorTile* Tile = Tiles[Index];

			// Se obtiene el coste de acceder al vecino y se comprueba que se tenga alcance y que sea accesible
			const int32 Cost = Tile->GetMovementCost();
			if (Cost <= Range && Tile->IsAccesible())
			{
				// Se trata de obtener el elemento de la casilla y se comprueba si es propiedad de la faccion actual
				const AActorDamageableElement* Element = Tile->GetElement();
				if (!Element || !Element->IsMine())
				{
					// Si es accesible, se anade a la lista y se obtienen todos sus vecinos que sean alcanzables
					InRange.Add(Neighbor);
					InRange.Append(GetTilesWithinRange(Neighbor, Range - Cost));
				}
			}
		}
	}

	return InRange;
}

//--------------------------------------------------------------------------------------------------------------------//

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
	case EMapSeaLevel::Arid: WaterProbabilityModifier = 0.1;
		break;
	case EMapSeaLevel::Standard: WaterProbabilityModifier = 0.13;
		break;
	case EMapSeaLevel::Wet: WaterProbabilityModifier = 0.17;
		break;
	}

	// Se declaran e inicializan los vectores necesarios para la generacion del mapa
	const int32 Dimension = Rows * Cols;
	TArray<FTileProbability> Probabilities;

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
			const FIntPoint Pos2D = FIntPoint(Row, Col);
			const ETileType TileType = GenerateTileType(Pos2D, Probabilities);

			UpdateTileAtPos(Pos2D, TileType);
		}
	}

	// Se actualizan los parametros de la instancia del juego para poder usarlos mas adelante
	UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		GameInstance->MapSize2D = FVector2D(GridSize.X, GridSize.Y - RowOffset);
		GameInstance->Size2D = FIntPoint(Rows, Cols);
	}
}

void AActorTileMap::UpdateTileAtPos(const FIntPoint& Pos, const ETileType TileType)
{
	// Se verifica que el indice que se obtiene es correcto
	const int32 Index = GetPositionInArray(Pos);
	if (Index == -1) return;

	// Se obtiene la casilla y, si existe y el tipo es diferente al dado, se destruye
	AActorTile* Tile = Tiles[Index];
	if (Tile && Tile->GetType() != TileType)
	{
		// Se destruye el actor
		Tile->Destroy();

		// Se obtiene el tipo previo de la casilla
		const ETileType PreviousTileType = TilesInfo[Pos].Type;

		// Se elimina la informacion de la casilla al diccionario que la almacena
		TilesInfo.Remove(Pos);
		// Se actualiza el diccionario que almacena el conteo de casillas por tipo
		TileTypeCount[PreviousTileType] -= 1;
		// Se elimina la referencia de la casilla eliminada
		Tiles[Index] = nullptr;

		// Se actualizan los datos de la nueva casilla
		SetTileAtPos(Pos, TileType);
	}
	// Si la casilla no existe se actualizan los datos de una nueva, en caso contrario, no se hace nada
	else if (!Tile) SetTileAtPos(Pos, TileType);
}

void AActorTileMap::DisplayTileAtPos(const TSubclassOf<AActorTile> Tile, const FTileInfo& TileInfo)
{
	// Se calcula la rotacion de la casilla que se va a anadir a la escena
	const int32 RotationSelector = FMath::RandRange(0, 5);
	const float Rotation = TileInfo.Type == ETileType::Mountains ? 180.0 / 3.0 * RotationSelector : 0.0;

	// Se anade la casilla a la escena
	AActorTile* NewTile = GetWorld()->SpawnActor<AActorTile>(
		Tile,
		FVector(TileInfo.MapPos2D.X, TileInfo.MapPos2D.Y, 0.0),
		FRotator(0, Rotation, 0));

	// Se actualizan los atributos de la casilla
	if (NewTile)
	{
		NewTile->SetPos(TileInfo.Pos2D, TileInfo.MapPos2D);
		NewTile->SetType(TileInfo.Type);

		// TODO quitar para lanzamiento
		NewTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), TileInfo.Pos2D.X, TileInfo.Pos2D.Y));
	}

	// Se actualiza el array de casillas con la que se ha anadido
	const int32 Index = GetPositionInArray(TileInfo.Pos2D);
	if (Index != -1) Tiles[Index] = NewTile;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorTileMap::AddResourceToTile(const FIntPoint& Pos, const FResource& Resource)
{
	// Se verifica que la posicion sea valida
	const int32 Index = GetPositionInArray(Pos);
	if (Index == -1) return;

	// Se obtiene la casilla
	AActorTile* Tile = Tiles[Index];

	// Se genera el nuevo recurso en la posicion de la casilla
	AActorResource* NewResource = GetWorld()->SpawnActor<AActorResource>(
		AActorResource::StaticClass(),
		FVector(Tile->GetMapPos().X, Tile->GetMapPos().Y, 0.0),
		FRotator(0.0));
	
	if (NewResource)
	{
		// Se actualizan los atributos del recurso
		NewResource->SetInfo(FResourceInfo(Tile->GetPos(), Resource));

		// Se actualiza el contador de recursos si la casilla ya contiene uno
		const AActorResource* ResourceInTile;
		if ((ResourceInTile = Tile->GetResource()) != nullptr) ResourceCount[ResourceInTile->GetResource()] -= 1;

		// Se asigna el recurso a la casilla
		Tile->SetResource(NewResource);

		// Se actualiza el contador de recursos
		ResourceCount[Resource.Resource] += 1;
	}
}

void AActorTileMap::RemoveResourceFromTile(const FIntPoint& Pos)
{
	// Se verifica que la posicion sea valida
	const int32 Index = GetPositionInArray(Pos);
	if (Index == -1) return;

	// Se obtiene la casilla
	AActorTile* Tile = Tiles[Index];

	// Se actualiza el contador de recursos
	const AActorResource* ResourceInTile;
	if ((ResourceInTile = Tile->GetResource()) != nullptr) ResourceCount[ResourceInTile->GetResource()] -= 1;

	// Se elimina el recurso de la casilla
	Tile->SetResource(nullptr);
}

void AActorTileMap::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorTileMap::SaveMap() const
{
	TArray<FMapSaveData> MapData;
	MapData.SetNumZeroed(Tiles.Num());

	if (USaveMap* SaveGameInstance = Cast<USaveMap>(UGameplayStatics::CreateSaveGameObject(USaveMap::StaticClass())))
	{
		SaveGameInstance->Tiles = MapData;
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
		SetMapFromSave(LoadedGame->Tiles);
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
		const int32 Type = AActorTile::TileTypeToInt(Tile->GetType());
		
		JsonData[i] = FMapDataForJson(Tile->GetMapPosition().X, Tile->GetMapPosition().Y, Type);
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

const TArray<FMovement>& AActorTileMap::FindPath(const FIntPoint& PosIni, const FIntPoint& PosEnd,
                                                 const int32 BaseMovementPoints, int32 MovementPoints)
{
	// Se limpian los datos anteriores
	Path.Empty();
	TotalCost.Empty();

	// Se llama al evento para que todos los suscriptores realicen las operaciones definidas
	OnPathCreated.Broadcast(TArray<FMovement>());

	// Si las dos casillas son iguales, se devuelve un array vacio
	if (PosIni == PosEnd) return Path;

	// Se comprueba que los datos son correctos, si no lo son, se devuelve un array vacio
	if (!(GetPositionInArray(PosIni) != -1 && GetPositionInArray(PosEnd) != -1))
	{
		return Path;
	}

	// Se comprueba que las casillas sean accesibles, si no lo son, se devuelve un array vacio
	if (!(Tiles[GetPositionInArray(PosIni)]->IsAccesible() && Tiles[GetPositionInArray(PosEnd)]->IsAccesible()))
	{
		return Path;
	}

	// Se comprueba que la unidad seleccionada no sea una unidad civil
	if (Cast<AActorCivilUnit>(Tiles[GetPositionInArray(PosIni)]->GetElement()))
	{
		// Si la unidad seleccionada es civil y en la casilla de destino hay una unidad civil propia,
		// se devuelve un array vacio
		if (Tiles[GetPositionInArray(PosEnd)]->GetElement()) return Path;
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

	// Se inicializa el diccionario de costes
	TotalCost.Add(PosIni, 0);

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
				const int32 Index = GetPositionInArray(Current);
				if (Index == -1) break;

				// Se obtiene el coste de movimiento de la casilla actual
				const int32 CurrentCost = Tiles[Index]->GetMovementCost();

				// Se anade el nodo actual al camino a devolver
				Path.Insert(FMovement(Current, CurrentCost, TotalCost[Current]), 0);

				// Se actualiza al siguiente nodo
				Current = CameFrom[Current];
			}

			// Se recorren todos los elementos del camino para llamar al evento que actualiza la visual del mapa
			for (int32 i = 0; i < Path.Num(); ++i)
			{
				// Se actualiza el numero de turnos en alcanzar la casilla actual
				ULibraryTileMap::GetPathTurnAtIndex(Path, i, BaseMovementPoints, MovementPoints);

				// Se llama al evento para que todos los suscriptores realicen las operaciones definidas
				OnPathUpdated.Broadcast(Path[i].Pos2D, Path);
			}

			break;
		}

		// Se calculan los vecinos de la casilla actual y se procesan
		const FIntPoint Limit = FIntPoint(Rows, Cols);
		for (const FIntPoint NeighborPos : ULibraryTileMap::GetNeighbors(CurrentData.Pos2D, Limit))
		{
			// Si el indice no es valido, se salta el vecino actual
			const int32 Index = GetPositionInArray(NeighborPos);
			if (Index == -1) continue;

			// Se verifica si el vecino calculado es correcto y, en caso de ser accesible, se obtiene
			const AActorTile* NeighborTile;
			if ((NeighborTile = Tiles[Index])->IsAccesible())
			{
				// Se trata de obtener el elemento de la casilla actual y, si lo tiene, solo se acepta si
				//		* es un asentamiento propio
				//		* se encuentra en la casilla de destino y no es propiedad de la faccion actual
				const AActorDamageableElement* Element = NeighborTile->GetElement();
				const bool IsMine = Element ? Element->IsMine() : false;
				if (!Element || (Cast<AActorSettlement>(Element) && IsMine) || (NeighborTile->GetPos() == PosEnd &&
					!IsMine))
				{
					// Se calcula el coste de llegar a esta casilla junto con el coste de movimiento de la propia casilla
					int32 NewCost = TotalCost[CurrentData.Pos2D] + NeighborTile->GetMovementCost();
					if (!TotalCost.Contains(NeighborPos))
					{
						// Si el nodo no se habia procesado previamente, se anade a las diferentes estructuras
						// con los valores de prioridad y coste correctos
						TotalCost.Add(NeighborPos, NewCost);

						const int32 Priority = NewCost + ULibraryTileMap::GetDistanceToElement(
							NeighborTile->GetPos(), PosEnd);
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

							const int32 Priority = NewCost + ULibraryTileMap::GetDistanceToElement(
								NeighborTile->GetPos(), PosEnd);
							Frontier.Push(FPathData(NeighborPos, Priority));

							CameFrom[NeighborPos] = CurrentData.Pos2D;
						}
					}
				}
			}
		}
	}

	return Path;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorTileMap::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

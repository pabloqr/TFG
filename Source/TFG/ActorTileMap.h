// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMovement.h"
#include "SaveMap.h"
#include "GameFramework/Actor.h"
#include "ActorTileMap.generated.h"

class AActorTile;

/**
 * Estructura que almacena una lista de coordenadas de casillas. Disenado para poder ser usado en diccionarios
 */
USTRUCT(BlueprintType)
struct FTilesArray
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TArray<FIntPoint> TilesArray;

	FTilesArray(): FTilesArray(TArray<FIntPoint>())
	{
	}

	explicit FTilesArray(const TArray<FIntPoint>& Tiles)
	{
		TilesArray = Tiles;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Estructura que almacena la probabilidad de aparicion de cada uno de los tipos de casillas
 */
USTRUCT()
struct FTileProbability
{
	GENERATED_BODY()

	float PlainsProbability = 0.f;
	float HillsProbability = 0.f;
	float ForestProbability = 0.f;
	float IceProbability = 0.f;
	float MountainsProbability = 0.f;
	float WaterProbability = 0.f;

	int32 Error = 0;
};

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Estructura que almacena informacion para el calculo de una ruta entre dos posiciones del mapa
 */
USTRUCT()
struct FPathData
{
	GENERATED_BODY()

	/**
	 * Posicion de la casilla
	 */
	FIntPoint Pos2D;
	/**
	 * Prioridad de la casilla para ser analizada por el algoritmo
	 */
	int32 Priority;

	/**
	 * Constructor por defecto. Establece los atributos a valores invalidos
	 */
	FPathData() : FPathData(FIntPoint(-1, -1), -1)
	{
	}

	/**
	 * Constructor con parametros
	 * 
	 * @param Pos Pos2D del nodo
	 * @param P Prioridad
	 */
	FPathData(const FIntPoint& Pos, const int32 P)
	{
		Pos2D = FIntPoint(Pos);
		Priority = P;
	}

	/**
	 * Operador <
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor que el que se compara
	 */
	bool operator<(const FPathData& Other) const
	{
		return Priority < Other.Priority;
	}

	/**
	 * Operador >
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor que el que se compara
	 */
	bool operator>(const FPathData& Other) const
	{
		return !(*this < Other || *this == Other);
	}

	/**
	 * Operador ==
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es igual que el que se compara
	 */
	bool operator==(const FPathData& Other) const
	{
		return Priority == Other.Priority;
	}

	/**
	 * Operador !=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es diferente que el que se compara
	 */
	bool operator!=(const FPathData& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * Operador <=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor o igual que el que se compara
	 */
	bool operator<=(const FPathData& Other) const
	{
		return *this < Other || *this == Other;
	}

	/**
	 * Operador >=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor o igual que el que se compara
	 */
	bool operator>=(const FPathData& Other) const
	{
		return *this > Other || *this == Other;
	}
};

/**
 * Clase para definir colas con prioridad
 */
class FPriorityQueue
{
	/**
	 * Lista de elementos parametrizada, el tipo de dato debe proporcionar los operadores implementados
	 */
	TArray<FPathData> Elements;

public:
	/**
	 * Metodo que devuelve si la cadena esta vacia o no
	 * 
	 * @return La cadena esta vacia
	 */
	bool Empty() const
	{
		return Elements.Num() == 0;
	}

	/**
	 * Metodo que inserta de forma ordenada el elemento dado segun su prioridad
	 * 
	 * @param Element Elemento a insertar
	 */
	void Push(const FPathData& Element)
	{
		// Se comprueba si el elemento tiene menos prioridad que el ultimo para evitar recorrer todo el array
		if (Empty() || Element >= Elements.Last()) Elements.Add(Element);
		else
			for (int32 i = 0; i < Elements.Num(); ++i)
			{
				// Si el elemento tiene menos prioridad se inserta y se finaliza
				if (Elements[i] >= Element)
				{
					Elements.Insert(Element, i);
					break;
				}
			}
	}

	/**
	 * Metodo que devuelve el elemento con mayor prioridad del array
	 * 
	 * @return El primer elemento del array
	 */
	FPathData Pop()
	{
		FPathData Element = Elements[0];
		Elements.RemoveAt(0);

		return Element;
	}

	/**
	 * Metodo que devuelve la representacion legible de la instancia de esta clase
	 * 
	 * @return Cadena a mostrar
	 */
	FString ToString()
	{
		FString Str;

		Str.Append(FString::Printf(TEXT("FrontierStart\n")));
		for (const FPathData Element : Elements)
		{
			Str.Append(FString::Printf(TEXT("(%d, %d) - P(%d)\n"), Element.Pos2D.X, Element.Pos2D.Y, Element.Priority));
		}
		Str.Append(FString::Printf(TEXT("FrontierEnd\n")));

		return Str;
	}
};

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileInfoUpdated, FIntPoint, Pos2D);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileUpdated, FTileInfo, TileInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceCreation, FResourceInfo, ResourceInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceCreated, AActorResource*, Resource);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathCreated, const TArray<FMovement>&, TilesToReset);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPathUpdated, FIntPoint, Pos2D, const TArray<FMovement>&, Path);

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Clase para la gestion de un mapa de casillas hexagonales
 */
UCLASS()
class TFG_API AActorTileMap : public AActor
{
	GENERATED_BODY()

protected:
	/**
	 * Array con informacion sobre el posicionamiento de las casillas y su tipo. Se emplea en los archivos de guardado
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Grid")
	TMap<FIntPoint, FTileInfo> TilesInfo;
	/**
	 * Array con referencias a las casillas del mapa
	 */
	UPROPERTY(BlueprintReadWrite, Category="Map|Grid")
	TArray<AActorTile*> Tiles;

	/**
	 * Diccionario que almacena, para cada estado, las casillas que lo tienen
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Grid")
	TMap<ETileState, FTilesArray> TilesWithState;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Grid")
	TMap<ETileType, int32> TileTypeCount;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Grid")
	TMap<EResource, int32> ResourceCount;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Posicion de la ultima casilla en coordenadas de la escena
	 */
	FVector2D GridSize;

	/**
	 * Numero de filas del mapa
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Rows;
	/**
	 * Numero de columnas del mapa
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	int32 Cols;

	/**
	 * Desplazamiento adicional para las filas impares
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float RowOffset;
	/**
	 * Desplazamiento horizontal entre casillas
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float HorizontalOffset;
	/**
	 * Desplazamiento vertical entre casillas
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Grid")
	float VerticalOffset;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Probabilidad de aparicion de casillas de agua en el mapa
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Grid")
	float WaterTileChance = 0.4f;

	/**
	 * Modificador de la probabilidad de aparicion de casillas de agua dependiente del atributo MapSeaLevel
	 */
	float WaterProbabilityModifier;

	/**
	 * Temperatura del mapa: determina el porcentaje de filas donde puede aparecer hielo y nieve en los polos del mapa
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Parameters")
	EMapTemperature MapTemperature;
	/**
	 * Nivel del mar del mapa: determina la cantidad de casillas de agua que apareceran en el mapa
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map|Parameters")
	EMapSeaLevel MapSeaLevel;

	/**
	 * Numero de filas donde puede aparecer hielo en los polos del mapa
	 */
	int32 NumIceRows;
	/**
	 * Numero de filas donde puede aparecer nieve en los polos del mapa
	 */
	int32 NumSnowRows;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Almacen del camino a seguir
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Pathfinding")
	TArray<FMovement> Path;

	/**
	 * Diccionario que almacena, para cada nodo, el coste de llegar a el
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Map|Pathfinding")
	TMap<FIntPoint, int32> TotalCost;

public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTileMap();

private:
	void ResetInfoStructures();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo privado que obtiene las coordenadas dentro del Array2D dada su posicion en el Array1D
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	FIntPoint GetCoordsInMap(const int32 Pos1D) const { return FIntPoint(GetRowInMap(Pos1D), GetColInMap(Pos1D)); }

	/**
	 * Metodo privado que obtiene la coordenada de la fila en el Array2D
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @return Valor de la fila en el Array2D
	 */
	int32 GetRowInMap(const int32 Pos1D) const { return Pos1D / Cols; }

	/**
	 * Metodo privado que obtiene la coordenada de la columna en el Array2D
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @return Valor de la columna en el Array2D
	 */
	int32 GetColInMap(const int32 Pos1D) const { return Pos1D % Cols; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que calcula la probabilidad de que una casilla sea Hielo (IceTile), se hara para que se acumule
	 * en los polos
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @param IceRow Indice dentro del numero de filas que pueden contener Hielo
	 * @return Probabilidad de que la casilla en la posicion dada pueda contener Hielo
	 */
	float ProbabilityOfIce(const int32 Pos1D, int32& IceRow) const;

	/**
	 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en las casillas
	 * circundantes a la actual
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param TileType Tipo de casilla a modificar
	 * @param Probability Variacion en el valor de la probabilidad
	 * @param Probabilities Array de probabilidades
	 */
	void UpdateProbability(const FIntPoint& Pos2D, const ETileType TileType, const float Probability,
	                       TArray<FTileProbability>& Probabilities) const;
	/**
	 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en una posicion
	 * concreta del Array2D
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param TileType Tipo de casilla a modificar
	 * @param Probability Variacion en el valor de la probabilidad
	 * @param Probabilities Array de probabilidades
	 */
	void UpdateProbabilityAtPos(const FIntPoint& Pos2D, const ETileType TileType, const float Probability,
	                            TArray<FTileProbability>& Probabilities) const;

	/**
	 * Metodo privado que calcula el tipo de casilla a generar en el mapa
	 * 
	 * @param Pos Coordenadas en el Array2D
	 * @param Probabilities Array de probabilidades de aparicion de los diferentes tipos de casillas
	 * @return Tipo de casilla a generar
	 */
	ETileType GenerateTileType(const FIntPoint& Pos, TArray<FTileProbability>& Probabilities) const;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo privado que actualiza la casilla deseada al tipo especificado
	 * 
	 * @param Pos2D Coordenadas en el Array2D
	 * @param TileType Tipo de casilla
	 */
	void SetTileAtPos(const FIntPoint& Pos2D, const ETileType TileType);

	/**
	 * Metodo privado que actualiza las casillas del mapa dada la informacion proporcionada del archivo de guardado
	 * 
	 * @param TilesData Array de Struct que contienen la informacion necesaria para establecer las casillas del mapa
	 * @param ResourcesData
	 */
	void SetMapFromSave(const TArray<FTileSaveData>& TilesData, const TArray<FResourceInfo>& ResourcesData);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo estatico que devuelve la cantidad a aplicar a la probabilidad para no llegar a un valor negativo
	 * 
	 * @param CurrentProbability Valor actual de la proabilidad de aparicion de un tipo de casilla
	 * @param NewProbability Cantidad que se quiere aplicar a la probabilidad
	 * @return Cantidad a aplicar a la probabilidad actual
	 */
	static float CheckProbability(const float CurrentProbability, const float NewProbability)
	{
		return CurrentProbability + NewProbability > 0.0 ? NewProbability : -CurrentProbability;
	}

protected:
	/**
	 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
	 * 
	 * @param Row Indice de la fila
	 * @param Col Indice de la columna
	 * @return Posicion en el Array1D
	 */
	int32 GetPositionInArray(const int32 Row, const int32 Col) const;

	/**
	 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 * @return Posicion en el Array1D
	 */
	UFUNCTION(BlueprintCallable)
	int32 GetPositionInArray(const FIntPoint& Pos) const { return GetPositionInArray(Pos.X, Pos.Y); }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que verifica si la casilla con las coordenadas dadas es accesible
	 * 
	 * @param Pos Coordenadas en el Array2D
	 * @return Si la casilla pedida es accesible
	 */
	UFUNCTION(BlueprintCallable)
	bool IsTileAccesible(const FIntPoint& Pos) const;

	/**
	 * Metodo que devuelve una casilla del mapa dada su posicion en el mismo
	 * 
	 * @param Pos Coordenadas en el Array2D
	 * @return Casilla del mapa
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	AActorTile* GetTileAtPos(const FIntPoint& Pos) const;

	/**
	 * Metodo que obtiene la lista de casillas con un estado dado
	 * 
	 * @param State Estado de las casillas que se quieren obtener
	 * @return Array con la Pos2D de las casillas que tienen el estado pedido
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<FIntPoint> GetTilesWithState(const ETileState& State) const;

	/**
	 * Metodo que obtiene la lista de casillas que se encuentran al alcance desde cierta posicion
	 * 
	 * @param Pos2D Coordenadas en el Array2D
	 * @param Range Alcance desde la posicion dada
	 * @return Coordenadas de las casillas que se encuentran dentro del rango desde la posicion dada
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetTilesWithinRange(const FIntPoint& Pos2D, const int32 Range);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que genera un mapa de forma aleatoria teniendo en cuenta los modificadores de temperatura y nivel del
	 * mar del mapa. El resultado sera un tablero de casillas que trata de asemejarse a la Tierra
	 * 
	 * @param Size2D Tamano del mapa
	 * @param Temperature Temperatura del mapa
	 * @param SeaLevel Nivel del mar del mapa
	 * @param WaterChance
	 */
	UFUNCTION(BlueprintCallable)
	void GenerateMap(const FIntPoint& Size2D, const EMapTemperature Temperature, const EMapSeaLevel SeaLevel,
	                 const float WaterChance);

	/**
	 * Metodo que calcula y devuelve posiciones validas para las unidades iniciales de las facciones
	 * 
	 * @return Posiciones iniciales para las facciones
	 */
	UFUNCTION(BlueprintCallable)
	TMap<int32, FTilesArray> GenerateStartingPositions(const int32 NumFactions);

	/**
	 * Metodo que actualiza la casilla dada su posicion teniendo en cuenta las casillas existentes
	 * 
	 * @param Pos Coordenadas en el Array2D
	 * @param TileType Tipo de casilla
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateTileAtPos(const FIntPoint& Pos, ETileType TileType);

	/**
	 * Metodo que crea y almacena una nueva casilla en el mapa
	 * 
	 * @param Tile Clase de la casilla
	 * @param TileInfo Informacion sobre la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void DisplayTileAtPos(const TSubclassOf<AActorTile> Tile, const FTileInfo& TileInfo);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddResourceToTile(const FIntPoint& Pos, const TSubclassOf<AActorResource> ResourceClass,
	                       const FResource& Resource);

	UFUNCTION(BlueprintCallable)
	void RemoveResourceFromTile(const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void AddUnitToTile(const FIntPoint& Pos, AActorUnit* Unit);

	UFUNCTION(BlueprintCallable)
	void RemoveUnitFromTile(const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void AddSettlementToTile(const FIntPoint& Pos, AActorSettlement* Settlement);

	UFUNCTION(BlueprintCallable)
	void RemoveSettlementFromTile(const FIntPoint& Pos);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que almacena la informacion de las casillas en un archivo de guardado para su posterior carga
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Save")
	void SaveMap(const FString CustomName = TEXT("")) const;

	/**
	 * Metodo que lee la informacion de las casillas de un archivo de guardado para actualizar el mapa
	 * 
	 * @param MapSaveData Informacion del archivo de guardado
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Save")
	void LoadMap(const FSaveData& MapSaveData);

	/**
	 * Metodo que elimina el archivo de guardado correspondiente al mapa seleccionado
	 * 
	 * @param MapSaveData Informacion del archivo de guardado
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Save")
	static void DeleteMap(const FSaveData& MapSaveData);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que transforma la informacion de las casillas para que pueda ser almacenada en un archivo Json
	 */
	// UFUNCTION(BlueprintCallable, Category="Map|Json")
	// void MapToJson();

	/**
	 * Metodo que transforma la informacion sobre las casillas de un archivo Json para actualizar el mapa
	 */
	// UFUNCTION(BlueprintCallable, Category="Map|Json")
	// void JsonToMap();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Metodo que calcula el mejor camino a seguir a lo largo del mapa para alcanzar una casilla del mismo
	 * 
	 * @param PosIni Posicion inicial del elemento
	 * @param PosEnd Posicion de destino del elemento
	 * @param BaseMovementPoints
	 * @param MovementPoints
	 * @return El mejor camino a seguir
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Pathfinding")
	const TArray<FMovement>& FindPath(const FIntPoint& PosIni, const FIntPoint& PosEnd, const int32 BaseMovementPoints,
	                                  const int32 MovementPoints);

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnTileInfoUpdated OnTileInfoUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnTileUpdated OnTileUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnResourceCreation OnResourceCreation;

	UPROPERTY(BlueprintAssignable)
	FOnResourceCreated OnResourceCreated;

	UPROPERTY(BlueprintAssignable)
	FOnPathCreated OnPathCreated;
	UPROPERTY(BlueprintAssignable)
	FOnPathUpdated OnPathUpdated;
};

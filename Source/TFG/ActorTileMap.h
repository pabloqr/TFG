// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorTileMap.generated.h"

struct FMapData;
enum class ETileType : uint8;
class AActorTile;

/**
 * Tipo enumerado para la clasificacion de mapas segun su temperatura
 */
UENUM(BlueprintType)
enum class EMapTemperature : uint8
{
	Desert = 0 UMETA(DisplayName="Desert"),
	Hot = 1 UMETA(DisplayName="Hot"),
	Temperate = 2 UMETA(DisplayName="Temperate"),
	Cold = 3 UMETA(DisplayName="Cold"),
};

/**
 * Tipo enumerado para la clasificacion de mapas segun su nivel del mar (cantidad de casillas de agua)
 */
UENUM(BlueprintType)
enum class EMapSeaLevel : uint8
{
	Arid = 0 UMETA(DisplayName="Arid"),
	Standard = 1 UMETA(DisplayName="Standard"),
	Wet = 2 UMETA(DisplayName="Wet"),
};

/**
 * Estructura que almacena la probabilidad de aparicion de cada uno de los tipos de casillas
 */
USTRUCT()
struct FSTileProbability
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

/**
 * Clase para la gestion de un mapa de casillas hexagonales
 */
UCLASS()
class TFG_API AActorTileMap : public AActor
{
	GENERATED_BODY()

protected:
	TArray<AActorTile*> Tiles;

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
	TSubclassOf<AActorTile> PlainsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> HillsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> ForestTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> SnowTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> IceTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> MountainsTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map|Tile")
	TSubclassOf<AActorTile> WaterTile;

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
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTileMap();

private:
	/**
	 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
	 * 
	 * @param Row Indice de la fila
	 * @param Col Indice de la columna
	 * @return Posicion en el Array1D
	 */
	int32 GetPositionInArray(const int32 Row, const int32 Col) const { return Row * Cols + Col; }
	
	/**
	 * Metodo privado que obtiene la posicion de una casilla dentro del Array1D dadas sus coordenadas en el Array2D
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @return Posicion en el Array1D
	 */
	int32 GetPositionInArray(const FIntPoint& Pos2D) const { return Pos2D.X * Cols + Pos2D.Y; }

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
	void UpdateProbability(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const;
	/**
	 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en una posicion
	 * concreta del Array2D
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param TileType Tipo de casilla a modificar
	 * @param Probability Variacion en el valor de la probabilidad
	 * @param Probabilities Array de probabilidades
	 */
	void UpdateProbabilityAtPos(const FIntPoint& Pos2D, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const;

	/**
	 * Metodo privado que devuelve la cantidad a aplicar a la probabilidad para no llegar a un valor negativo
	 * 
	 * @param CurrentProbability Valor actual de la proabilidad de aparicion de un tipo de casilla
	 * @param NewProbability Cantidad que se quiere aplicar a la probabilidad
	 * @return Cantidad a aplicar a la probabilidad actual
	 */
	float CheckProbability(const float CurrentProbability, const float NewProbability) const
	{
		return CurrentProbability+NewProbability > 0.0 ? NewProbability : -CurrentProbability;
	}
	
	/**
	 * Metodo privado que calcula el tipo de casilla a generar en el mapa
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @param Pos2D Coordenadas en el Array2D
	 * @param Probabilities Array de probabilidades de aparicion de los diferentes tipos de casillas
	 * @return Tipo de casilla a generar
	 */
	ETileType GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FSTileProbability>& Probabilities) const;

	/**
	 * Metodo privado que selecciona la instancia a generar en el mapa dado el tipo de casilla deseado
	 * 
	 * @param TileType Tipo de casilla
	 * @return Instancia del tipo de casilla a generar en el mapa
	 */
	TSubclassOf<AActorTile> SelectTileType(ETileType TileType) const;

	/**
	 * Metodo privado que actualiza la casilla deseada al tipo especificado
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @param Pos2D Coordenadas en el Array2D
	 * @param TileType Tipo de casilla
	 */
	void SetTileAtPos(const int32 Pos1D, const FIntPoint& Pos2D, const ETileType TileType);

	/**
	 * Metodo privado que actualiza las casillas del mapa dada la informacion proporcionada del archivo de guardado
	 * 
	 * @param TilesInfo Array de Struct que contienen la informacion necesaria para establecer las casillas del mapa
	 */
	void SetMapFromSave(const TArray<FMapData>& TilesInfo);

protected:
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Metodo que almacena la informacion de las casillas en un archivo de guardado para su posterior carga
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Save")
	void SaveMap() const;

	/**
	 * Metodo que lee la informacion de las casillas de un archivo de guardado para actualizar el mapa
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Save")
	void LoadMap();

	/**
	 * Metodo que transforma la informacion de las casillas para que pueda ser almacenada en un archivo Json
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Json")
	void MapToJson();

	/**
	 * Metodo que transforma la informacion sobre las casillas de un archivo Json para actualizar el mapa
	 */
	UFUNCTION(BlueprintCallable, Category="Map|Json")
	void JsonToMap();
};

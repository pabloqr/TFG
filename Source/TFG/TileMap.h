// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMap.generated.h"

enum class ETileType : uint8;
class ATile;

/**
 * Estructura que almacena la informacion sobre las casillas a guardar en un archivo de texto
 */
USTRUCT()
struct FJsonMapDataEntry
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Row;
	UPROPERTY()
	int32 Col;
	UPROPERTY()
	int32 TileType;

	FJsonMapDataEntry()
	{
		Row = -1;
		Col = -1;
		TileType = -1;
	}

	FJsonMapDataEntry(const int32 Row, const int32 Col, const int32 TileType)
	{
		this->Row = Row;
		this->Col = Col;
		this->TileType = TileType;
	}
};

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
	float SnowProbability = 0.f;
	float IceProbability = 0.f;
	float MountainsProbability = 0.f;
	float WaterProbability = 0.f;
	
	int Error = 0;
};

/**
 * Clase para la gestion de un mapa de casillas hexagonales
 */
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
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	ATileMap();

private:
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
	 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @return Posicion en el Array1D
	 */
	int32 GetPositionInArray(const FIntPoint& Pos) const;

	/**
	 * Metodo privado que obtiene las coordenadas dentro del Array2D dada su posicion en el Array1D
	 * 
	 * @param Pos Posicion en el Array1D
	 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	FIntPoint GetCoordsInMap(const int32 Pos) const;
	/**
	 * Metodo privado que obtiene la coordenada de la fila en el Array2D
	 * 
	 * @param Pos Posicion en el Array1D
	 * @return Valor de la fila en el Array2D
	 */
	int32 GetRowInMap(const int32 Pos) const;
	/**
	 * Metodo privado que obtiene la coordenada de la columna en el Array2D
	 * 
	 * @param Pos Posicion en el Array1D
	 * @return Valor de la columna en el Array2D
	 */
	int32 GetColInMap(const int32 Pos) const;

	/**
	 * Metodo que calcula la probabilidad de que una casilla sea Hielo (IceTile), se hara para que se acumule
	 * en los polos
	 * 
	 * @param Pos Posicion en el Array1D
	 * @param IceRow Indice dentro del numero de filas que pueden contener Hielo
	 * @return Probabilidad de que la casilla en la posicion dada pueda contener Hielo
	 */
	float ProbabilityOfIce(const int32 Pos, int32& IceRow) const;

	/**
	 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en las casillas
	 * circundantes a la actual
	 * 
	 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param TileType Tipo de casilla a modificar
	 * @param Probability Variacion en el valor de la probabilidad
	 * @param Probabilities Array de probabilidades
	 */
	void UpdateProbability(const FIntPoint& Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const;
	/**
	 * Metodo privado que actualiza el valor de la probabilidad de aparicion de un tipo de casilla en una posicion
	 * concreta del Array2D
	 * 
	 * @param Pos Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param TileType Tipo de casilla a modificar
	 * @param Probability Variacion en el valor de la probabilidad
	 * @param Probabilities Array de probabilidades
	 */
	void UpdateProbabilityAtPos(const FIntPoint& Pos, const ETileType TileType, const float Probability, TArray<FSTileProbability>& Probabilities) const;

	/**
	 * Metodo privado que calcula el tipo de casilla a generar en el mapa
	 * 
	 * @param Pos1D Posicion en el Array1D
	 * @param Pos2D Coordenadas en el Array2D
	 * @param Probabilities Array de probabilidades de aparicion de los diferentes tipos de casillas
	 * @return Tipo de casilla a generar
	 */
	ETileType GenerateTileType(const int32 Pos1D, const FIntPoint& Pos2D, TArray<FSTileProbability>& Probabilities) const;

protected:
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Map|Json")
	void MapToJson();

	UFUNCTION(BlueprintCallable, Category="Map|Json")
	void JsonToMap();
	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
};

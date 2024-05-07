// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorTile.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Plains = 1 UMETA(DisplayName="Plains"),
	Hills = 2 UMETA(DisplayName="Hills"),
	Forest = 3 UMETA(DisplayName="Forest"),
	SnowPlains = 4 UMETA(DisplayName="SnowPlains"),
	SnowHills = 5 UMETA(DisplayName="SnowHills"),
	Ice = 6 UMETA(DisplayName="Ice"),
	Mountains = 7 UMETA(DisplayName="Mountains"),
	Water = 8 UMETA(DisplayName="Water"),
	Max = 255 UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETileState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Hovered = 1 UMETA(DisplayName="Hovered"),
	Selected = 2 UMETA(DisplayName="Selected")
};

USTRUCT(BlueprintType)
struct FTileInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	FVector2D MapPos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	ETileType TileType;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	TArray<ETileState> TileStates;

	FTileInfo(): FTileInfo(
		FIntPoint(0, 0),
		FVector2D(0.0, 0.0),
		ETileType::None,
		ETileState::None) {}

	FTileInfo(const FIntPoint& Pos2D, const FVector2D& MapPos2D, const ETileType TileType, const ETileState TileState = ETileState::None)
	{
		this->Pos2D = FIntPoint(Pos2D);
		this->MapPos2D = FVector2D(MapPos2D);
		this->TileType = TileType;
		this->TileStates.Add(TileState);
	}
};

UCLASS()
class TFG_API AActorTile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile")
	FTileInfo TileInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent* TileMesh;
	
public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTile();

	/**
	 * Metodo que transforma el tipo de casilla en un valor numerico
	 * 
	 * @param TileType Tipo de casilla
	 * @return Valor entero dado el tipo de casilla
	 */
	static int32 TileTypeToInt(const ETileType TileType);
	
	/**
	 * Metodo que transforma un valor numerico en el tipo de casilla
	 * 
	 * @param TileTypeVal Valor numerico del tipo de casilla
	 * @return Tipo de casilla
	 */
	static ETileType IntToTileType(const int32 TileTypeVal);

	/**
	 * Getter del atributo Info
	 * 
	 * @return Estructura que contiene toda la informacion sobre la casilla
	 */
	FTileInfo GetInfo() const;

	/**
	 * Getter del atributo MapPosition
	 * 
	 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	FIntPoint GetMapPosition() const;

	/**
	 * Getter del atributo MapPosition
	 * 
	 * @return Pareja de valores con las coordenadas de la fila y la columna en la esncena
	 */
	FVector2D GetScenePosition() const;

	/**
	 * Getter del atributo TileType
	 * 
	 * @return Tipo de casilla
	 */
	ETileType GetType() const;

	/**
	 * Getter del atributo TileStatess
	 * 
	 * @return Estado de la casilla
	 */
	TArray<ETileState> GetState() const;

	/**
	 * Setter de la informacion de la casilla
	 * 
	 * @param Info Informacion sobre la casilla
	 */
	void SetInfo(const FTileInfo& Info);

	/**
	 * Setter del atributo MapPosition
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	void SetPosition(const FIntPoint& Pos2D);
	/**
	 * Setter del atributo MapPosition
	 * 
	 * @param MapPos2D Pareja de valores con las coordenadas de la fila y la columna en la escena
	 */
	void SetPosition(const FVector2D& MapPos2D);
	/**
	 * Setter del atributo MapPosition
	 * 
	 * @param Pos2D Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 * @param MapPos2D Pareja de valores con las coordenadas de la fila y la columna en la escena
	 */
	void SetPosition(const FIntPoint& Pos2D, const FVector2D& MapPos2D);

	/**
	 * Setter del atributo TileType
	 * 
	 * @param TileType Tipo de casilla
	 */
	void SetType(const ETileType TileType);

	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param TileStates Estados de la casilla
	 */
	void SetState(const TArray<ETileState>& TileStates);
	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param TileState Estado de la casilla
	 */
	void SetState(const ETileState TileState);

	/**
	 * Anade los estados dados a los actuales
	 * 
	 * @param TileStates Estados de la casilla
	 */
	void AddState(const TArray<ETileState>& TileStates);
	/**
	 * Anade el estado dado a los actuales
	 * 
	 * @param TileState Estado de la casilla
	 */
	void AddState(const ETileState TileState);

	/**
	 * Elimina los estados dados de los actuales
	 * 
	 * @param TileStates Estados de la casilla
	 */
	void RemoveState(const TArray<ETileState>& TileStates);
	/**
	 * Elimina el estado dado de los actuales
	 * 
	 * @param TileState Estado de la casilla
	 */
	void RemoveState(const ETileState TileState);
};

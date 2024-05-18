// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPlaceableElement.h"
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

	FTileInfo(): FTileInfo(
		FIntPoint(0, 0),
		FVector2D(0.0, 0.0),
		ETileType::None) {}

	FTileInfo(const FIntPoint& Pos2D, const FVector2D& MapPos2D, const ETileType TileType)
	{
		this->Pos2D = FIntPoint(Pos2D);
		this->MapPos2D = FVector2D(MapPos2D);
		this->TileType = TileType;
	}
};

UCLASS()
class TFG_API AActorTile : public AActorPlaceableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	ETileType TileType;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	int32 MovementCost;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	TArray<ETileState> TileStates;
	
public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTile();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que devuelve la informacion serializada de la casilla
	 * 
	 * @return Informacion sobre la casilla
	 */
	FTileInfo GetInfo() const;
	
	/**
	 * Getter del atributo TileType
	 * 
	 * @return Tipo de casilla
	 */
	const ETileType& GetType() const;

	/**
	 * Getter del atributo MovementCost
	 * 
	 * @return Coste de paso por la casilla
	 */
	int32 GetMovementCost() const;

	/**
	 * Getter del atributo TileStatess
	 * 
	 * @return Estado de la casilla
	 */
	const TArray<ETileState>& GetState() const;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter del atributo Pos2D y MapPos2D
	 * 
	 * @param Pos Estructura que contiene la posicion en el tablero y en la escena
	 */
	void SetPos(const FTileInfo& Pos);

	/**
	 * Setter del atributo TileType
	 * 
	 * @param Type Tipo de casilla
	 */
	void SetType(const ETileType Type);

	/**
	 * Setter del atributo MovementCost
	 * 
	 * @param Cost Coste de paso por la casilla
	 */
	void SetMovementCost(const int32 Cost);

	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param States Estados de la casilla
	 */
	void SetState(const TArray<ETileState>& States);
	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param State Estado de la casilla
	 */
	void SetState(const ETileState State);

	/**
	 * Anade los estados dados a los actuales
	 * 
	 * @param States Estados de la casilla
	 */
	void AddState(const TArray<ETileState>& States);
	/**
	 * Anade el estado dado a los actuales
	 * 
	 * @param State Estado de la casilla
	 */
	void AddState(const ETileState State);

	/**
	 * Elimina los estados dados de los actuales
	 * 
	 * @param States Estados de la casilla
	 */
	void RemoveState(const TArray<ETileState>& States);
	/**
	 * Elimina el estado dado de los actuales
	 * 
	 * @param State Estado de la casilla
	 */
	void RemoveState(const ETileState State);

	//----------------------------------------------------------------------------------------------------------------//

	bool IsAccesible() const;

	//----------------------------------------------------------------------------------------------------------------//

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
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTileInfo.generated.h"

class AActorSettlement;
class AActorUnit;
class AActorResource;

/**
 * Tipo enumerado para determinar el tipo de una casilla
 */
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
	Water = 8 UMETA(DisplayName="Water")
};

/**
 * Tipo enumerado para determinar el estado de una casilla
 */
UENUM(BlueprintType)
enum class ETileState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Hovered = 1 UMETA(DisplayName="Hovered"),
	HoveredWithFriendElement = 2 UMETA(DisplayName="HoveredWithFriendElement"),
	HoveredWithEnemyElement = 3 UMETA(DisplayName="HoveredWithEnemyElement"),
	Selected = 4 UMETA(DisplayName="Selected"),
	Owned = 5 UMETA(DisplayName="Owned"),
	Neighbor = 6 UMETA(DisplayName="Neighbor"),
	Reachable = 7 UMETA(DisplayName="Reachable"),
	InPath = 8 UMETA(DisplayName="InPath")
};

/**
 * Estructura que almacena informacion sobre los elementos de la partida situados sobre la casilla
 */
USTRUCT(BlueprintType)
struct FTileElements
{
	GENERATED_BODY()

	/**
	 * Recurso sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	AActorResource* Resource;

	/**
	 * Unidad sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	AActorUnit* Unit;

	/**
	 * Asentamiento sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	AActorSettlement* Settlement;

	/**
	 * Constructor por defecto
	 */
	FTileElements(): FTileElements(nullptr, nullptr, nullptr)
	{
	}

	/**
	 * Constructor con parametros
	 * 
	 * @param ResourceAtTile Recurso situado en la casilla
	 * @param UnitAtTile Unidad situada en la casilla
	 * @param SettlementAtTile Asentamiento situado en la casilla
	 */
	FTileElements(AActorResource* ResourceAtTile, AActorUnit* UnitAtTile, AActorSettlement* SettlementAtTile)
	{
		Resource = ResourceAtTile;
		Unit = UnitAtTile;
		Settlement = SettlementAtTile;
	}
};

/**
 * Estructura que almacena informacion sobre las casillas para los archivos de guardado
 */
USTRUCT(BlueprintType)
struct FTileInfo
{
	GENERATED_BODY()

	/**
	 * Posicion en el Array2D que representa el mapa
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Position")
	FIntPoint Pos2D;
	/**
	 * Posicion en coordenadas de la escena
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Position")
	FVector2D MapPos2D;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Indice de la faccion propietaria de la casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	int32 Owner;

	/**
	 * Tipo de casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	ETileType Type;

	/**
	 * Elementos del juego contenidos en la casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Elements")
	FTileElements Elements;

	/**
	 * Estados de la casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	TArray<ETileState> States;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Constructor por defecto
	 */
	FTileInfo(): FTileInfo(FIntPoint(-1), FVector2D(0.0), -1, ETileType::None, FTileElements(),
	                       {ETileState::None})
	{
	}

	/**
	 * Constructor con parametros
	 * 
	 * @param Pos2D Posicion en el Array2D de casillas
	 * @param MapPos2D Posicion en coordenadas de la escena
	 * @param Owner Faccion propietaria de la casilla
	 * @param Type Tipo de casilla
	 * @param Elements Elementos del juego presentes en la casilla
	 * @param States Estado de la casilla
	 */
	FTileInfo(const FIntPoint& Pos2D, const FVector2D& MapPos2D, const int32 Owner, const ETileType Type,
	          const FTileElements& Elements, const TArray<ETileState>& States)
		: Pos2D(Pos2D),
		  MapPos2D(MapPos2D),
		  Owner(Owner),
		  Type(Type),
		  Elements(Elements),
		  States(States)
	{
	}
};

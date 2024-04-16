// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>

/**
 * Constructor de la clase que inicializa los parametros del actor
 */
ATile::ATile()
{
 	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);

	MapPosition = FIntPoint(-1, -1);
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo que transforma el tipo de casilla en un valor numerico
 * 
 * @param TileType Tipo de casilla
 * @return Valor entero dado el tipo de casilla
 */
int32 ATile::TileTypeToInt(const ETileType TileType)
{
	int32 TileTypeVal;
	switch (TileType)
	{
		case ETileType::Plains: TileTypeVal = 1; break;
		case ETileType::Hills: TileTypeVal = 2; break;
		case ETileType::Forest: TileTypeVal = 3; break;
		case ETileType::SnowPlains: TileTypeVal = 4; break;
		case ETileType::SnowHills: TileTypeVal = 5; break;
		case ETileType::Ice: TileTypeVal = 6; break;
		case ETileType::Mountains: TileTypeVal = 7; break;
		case ETileType::Water: TileTypeVal = 8; break;
		default: TileTypeVal = 0; break;
	}

	return TileTypeVal;
}

/**
 * Metodo que transforma un valor numerico en el tipo de casilla
 * 
 * @param TileTypeVal Valor numerico del tipo de casilla
 * @return Tipo de casilla
 */
ETileType ATile::IntToTileType(const int32 TileTypeVal)
{
	ETileType TileType;
	switch (TileTypeVal)
	{
		case 1: TileType = ETileType::Plains; break;
		case 2: TileType = ETileType::Hills; break;
		case 3: TileType = ETileType::Forest; break;
		case 4: TileType = ETileType::SnowPlains; break;
		case 5: TileType = ETileType::SnowHills; break;
		case 6: TileType = ETileType::Ice; break;
		case 7: TileType = ETileType::Mountains; break;
		case 8: TileType = ETileType::Water; break;
		default: TileType = ETileType::None; break;
	}

	return TileType;
}

/**
 * Getter del atributo MapPosition
 * 
 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 */
FIntPoint ATile::GetMapPosition() const
{
	return MapPosition;
}

/**
 * Getter del atributo TileType
 * 
 * @return Tipo de casilla
 */
ETileType ATile::GetTileType() const
{
	return TileType;
}

/**
 * Setter del atributo MapPosition
 * 
 * @param Position Pareja de valores con las coordenadas de la fila y la columna en el Array2D
 */
void ATile::SetPosition(const FIntPoint& Position)
{
	MapPosition.X = Position.X;
	MapPosition.Y = Position.Y;
}

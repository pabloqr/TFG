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
}

//--------------------------------------------------------------------------------------------------------------------//

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
	this->MapPosition = Position;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
// void ATile::BeginPlay()
// {
//	Super::BeginPlay();
// }

// Called every frame
// void ATile::Tick(float DeltaTime)
// {
//	Super::Tick(DeltaTime);
// }

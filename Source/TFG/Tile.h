// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Plains = 1 UMETA(DisplayName="PLains"),
	Hills = 2 UMETA(DisplayName="Hills"),
	Forest = 3 UMETA(DisplayName="Forest"),
	SnowPlains = 4 UMETA(DisplayName="SnowPlains"),
	SnowHills = 5 UMETA(DisplayName="SnowHills"),
	Ice = 6 UMETA(DisplayName="Ice"),
	Mountains = 7 UMETA(DisplayName="Mountains"),
	Water = 8 UMETA(DisplayName="Water"),
	Max = 255 UMETA(Hidden)
};

UCLASS()
class TFG_API ATile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile")
	FIntPoint MapPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile")
	ETileType TileType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent *TileMesh;
	
public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	ATile();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	/**
	 * Getter del atributo MapPosition
	 * 
	 * @return Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	FIntPoint GetMapPosition() const;

	/**
	 * Getter del atributo TileType
	 * 
	 * @return Tipo de casilla
	 */
	ETileType GetTileType() const;

	/**
	 * Setter del atributo MapPosition
	 * 
	 * @param Position Pareja de valores con las coordenadas de la fila y la columna en el Array2D
	 */
	void SetPosition(const FIntPoint& Position);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMovement.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LibraryTileMap.generated.h"

enum class ETileType : uint8;
/**
 * 
 */
UCLASS()
class TFG_API ULibraryTileMap : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Metodo estatico que verifica si una posicion se encuentra dentro de ciertos limites
	 * 
	 * @param Pos Posicion que se quiere comprobar
	 * @param Limit Limite maximo de posicion
	 * @return Si la posicion es valida
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool CheckValidPosition(const FIntPoint& Pos, const FIntPoint& Limit);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsTileTypeAccesible(const ETileType& TileType);

	UFUNCTION(BlueprintCallable)
	static int32 IsTileInPath(const TArray<FMovement>& Path, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	static void UpdatePathTurns(TArray<FMovement>& Path, const int32 BaseMovementPoints, int32 MovementPoints);

	UFUNCTION(BlueprintCallable)
	static void GetPathTurnAtIndex(TArray<FMovement>& Path, const int32 Index, const int32 BaseMovementPoints, int32& MovementPoints);
};

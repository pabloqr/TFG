// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LibraryTileMap.generated.h"

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
	UFUNCTION(BlueprintCallable)
	static bool CheckValidPosition(const FIntPoint& Pos, const FIntPoint& Limit)
	{
		return 0 <= Pos.X && Pos.X < Limit.X && 0 <= Pos.Y && Pos.Y < Limit.Y;
	}
};

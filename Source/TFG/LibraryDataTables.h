// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDataTableStructures.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LibraryDataTables.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API ULibraryDataTables : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FTileData GetTileDataFromType(const UDataTable* DataTable, const ETileType& TileType);
	
	UFUNCTION(BlueprintCallable)
	static FResourceData GetResourceDataFromType(const UDataTable* DataTable, const EResource& Resource);
};

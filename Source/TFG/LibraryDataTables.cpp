// Fill out your copyright notice in the Description page of Project Settings.


#include "LibraryDataTables.h"

FTileData ULibraryDataTables::GetTileDataFromType(const UDataTable* DataTable, const ETileType& TileType)
{
	// Si es no es un tipo valido, se devuelve una estructura nula
	if (TileType == ETileType::None) return FTileData();
	
	// Se obtiene el nombre del tipo enumerado seleccionado
	TArray<FString> ParsedEnum;
	UEnum::GetValueAsString(TileType).ParseIntoArray(ParsedEnum, TEXT(":"), false);

	// Se obtiene la fila usando el nombre del tipo enumerado
	const auto TileData = DataTable->FindRow<FTileData>(*ParsedEnum.Last(), TEXT(""));
	return TileData ? FTileData(TileData->Name, TileData->Type, TileData->MovementCost, TileData->Icon) : FTileData();
}

FResourceData ULibraryDataTables::GetResourceDataFromType(const UDataTable* DataTable, const EResource& Resource)
{
	// Si es no es un tipo valido, se devuelve una estructura nula
	if (Resource == EResource::None) return FResourceData();
	
	// Se obtiene el nombre del tipo enumerado seleccionado
	TArray<FString> ParsedEnum;
	UEnum::GetValueAsString(Resource).ParseIntoArray(ParsedEnum, TEXT(":"), false);

	// Se obtiene la fila usando el nombre del tipo enumerado
	const auto ResourceData = DataTable->FindRow<FResourceData>(*ParsedEnum.Last(), TEXT(""));
	return ResourceData
		       ? FResourceData(ResourceData->Name, ResourceData->Resource, ResourceData->Icon)
		       : FResourceData();
}

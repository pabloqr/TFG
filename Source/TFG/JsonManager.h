// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonManager.generated.h"

/**
 * Estructura que almacena la informacion sobre las casillas a guardar en un archivo de texto
 */
USTRUCT(BlueprintType, Category="Saves|Json")
struct FMapDataForJson
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Row;
	UPROPERTY()
	int32 Col;
	UPROPERTY()
	int32 TileType;

	FMapDataForJson()
	{
		Row = -1;
		Col = -1;
		TileType = -1;
	}

	FMapDataForJson(const int32 Row, const int32 Col, const int32 TileType)
	{
		this->Row = Row;
		this->Col = Col;
		this->TileType = TileType;
	}
};

const FString MapSavePath = "C:/Users/Pablo/Documents/Unreal Savefiles/TFG/Maps";
const FString SavesSavePath = "C:/Users/Pablo/Documents/Unreal Savefiles/TFG/Saves";

/**
 * Clase para la gestion de archivos Json
 */
UCLASS()
class TFG_API UJsonManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/**
	 * Metodo privado estatico que verifica que la extension para el archivo es correcta
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @return Ruta comprobada del archivo Json
	 */
	static FString CheckJsonExtension(const FString& JsonPath);
	/**
	 * Metodo privado estatico que verifica que la ruta para el archivo es correcta
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @return Ruta comprobada del archivo Json
	 */
	static FString CheckMapPath(const FString& JsonPath);
	
	/**
	 * Metodo privado estatico que abre un archivo Json, lo lee y lo convierte a un JsonObject para ser procesado
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 * @return El JsonObject con el contenido del archivo
	 */
	static TSharedPtr<FJsonObject> ReadJson(const FString& JsonPath, bool& Success, FString& ResultMessage);

	/**
	 * Metodo privado estatico que convierte un JsonObject a una cadena de texto, crea un archivo Json
	 * y escribe en el la cadena
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param JsonObject Objeto que contiene la informacion a escribir en el archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 */
	static void WriteJson(const FString& JsonPath, const TSharedPtr<FJsonObject>& JsonObject, bool& Success, FString& ResultMessage);

public:
	/**
	 * Metodo estatico que transforma el JsonObject en una estructura mejor manipulable
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 * @return Lista que contiene la informacion del archivo Json
	 */
	UFUNCTION(BlueprintCallable, Category="FileManager|Json")
	static TArray<FMapDataForJson> JsonToMapStruct(FString JsonPath, bool& Success, FString& ResultMessage);

	/**
	 * Metodo estatico que transforma la estructura con la informacion a almacenar en un JsonObject
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param JsonData Lista que contiene la informacion del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 */
	UFUNCTION(BlueprintCallable, Category="FileManager|Json")
	static void MapStructToJson(FString JsonPath, const TArray<FMapDataForJson>& JsonData, bool& Success, FString& ResultMessage);
};

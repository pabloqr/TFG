// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonManager.generated.h"

/**
 * Estructura que almacena la lista de casillas del mapa
 */
USTRUCT(BlueprintType, Category="FileManager|Json")
struct FJsonMapData
{
	GENERATED_BODY()

public:
	TArray<TSharedPtr<FJsonValue>> Tiles;
};

/**
 * Clase para la gestion de archivos Json
 */
UCLASS()
class TFG_API UJsonManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	/**
	 * Metodo privado estatico que abre un archivo Json, lo lee y lo convierte a un JsonObject para ser procesado
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 * @return El JsonObject con el contenido del archivo
	 */
	static TSharedPtr<FJsonObject> ReadJson(FString JsonPath, bool& Success, FString& ResultMessage);

	/**
	 * Metodo privado estatico que convierte un JsonObject a una cadena de texto, crea un archivo Json
	 * y escribe en el la cadena
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param JsonObject Objeto que contiene la informacion a escribir en el archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 */
	static void WriteJson(FString JsonPath, const TSharedPtr<FJsonObject>& JsonObject, bool& Success, FString& ResultMessage);

public:
	/**
	 * Metodo estatico que transforma el JsonObject en una estructura mejor manipulable
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 * @return Estructura que contiene la informacion del archivo Json
	 */
	UFUNCTION(BlueprintCallable, Category="FileManager|Json")
	static FJsonMapData JsonToMapStruct(FString JsonPath, bool& Success, FString& ResultMessage);

	/**
	 * Metodo estatico que transforma la estructura con la informacion a almacenar en un JsonObject
	 * 
	 * @param JsonPath Ruta del archivo Json
	 * @param JsonStructure Estructura que contiene la informacion del archivo Json
	 * @param Success Resultado de la operacion
	 * @param ResultMessage Informacion de la operacion
	 */
	UFUNCTION(BlueprintCallable, Category="FileManager|Json")
	static void MapStructToJson(FString JsonPath, const FJsonMapData& JsonStructure, bool& Success, FString& ResultMessage);
};

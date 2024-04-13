// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonManager.h"
#include "FileManager.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"

/**
 * Metodo privado estatico que abre un archivo Json, lo lee y lo convierte a un JsonObject para ser procesado
 * 
 * @param JsonPath Ruta del archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 * @return El JsonObject con el contenido del archivo
 */
TSharedPtr<FJsonObject> UJsonManager::ReadJson(FString JsonPath, bool& Success, FString& ResultMessage)
{
	// Se intenta leer el archivo dado
	FString JsonString = UFileManager::ReadStringFromFile(JsonPath, Success, ResultMessage);
	if (!Success) return nullptr;

	TSharedPtr<FJsonObject> JsonObject;

	// Se intenta convertir la cadena obtenida en un JsonObjec
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al deserializar la cadena con la informacion Json\n%s"), *JsonPath, *JsonString);
		return nullptr;
	}

	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Creacion de JsonObject correcta"), *JsonPath);
	return JsonObject;
}

/**
 * Metodo privado estatico que convierte un JsonObject a una cadena de texto, crea un archivo Json
 * y escribe en el la cadena
 * 
 * @param JsonPath Ruta del archivo Json
 * @param JsonObject Objeto que contiene la informacion a escribir en el archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 */
void UJsonManager::WriteJson(FString JsonPath, TSharedPtr<FJsonObject> JsonObject, bool& Success, FString& ResultMessage)
{
	FString JsonString;

	// Se intenta convertir el JsonObject en una cadena
	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al serializar la cadena con la informacion Json"), *JsonPath);
		return;
	}

	UFileManager::WriteStringToFile(JsonPath, JsonString, Success, ResultMessage);
	if (!Success) return;

	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Escritura del archivo Json correcta"), *JsonPath);
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo estatico que transforma el JsonObject en una estructura mejor manipulable
 * 
 * @param JsonPath Ruta del archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 * @return Estructura que contiene la informacion del archivo Json
 */
FJsonMapData UJsonManager::JsonToStruct(FString JsonPath, bool& Success, FString& ResultMessage)
{
	// Se intenta crear un JsonObject de un archivo
	TSharedPtr<FJsonObject> JsonObject = ReadJson(JsonPath, Success, ResultMessage);
	if (!Success) return FJsonMapData();

	FJsonMapData JsonData;

	// Se intenta convertir el JsonObject en la estructura creada
	if (!FJsonObjectConverter::JsonObjectToUStruct<FJsonMapData>(JsonObject.ToSharedRef(), &JsonData))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al convertir el objeto Json a la estructura interna"), *JsonPath);
		return FJsonMapData();
	}

	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Conversion a la estructura interna correcta"), *JsonPath);
	return JsonData;
}

/**
 * Metodo estatico que transforma la estructura con la informacion a almacenar en un JsonObject
 * 
 * @param JsonPath Ruta del archivo Json
 * @param JsonStructure Estructura que contiene la informacion del archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 */
void UJsonManager::StructToJson(FString JsonPath, FJsonMapData JsonStructure, bool& Success, FString& ResultMessage)
{
	// Se intenta convertir la estructura en el JsonObject
	TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(JsonStructure);
	if (JsonObject == nullptr)
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al convertir la estructura interna al objeto Json"), *JsonPath);
		return;
	}

	// Se intenta escribir los datos al archivo
	WriteJson(JsonPath, JsonObject, Success, ResultMessage);
}

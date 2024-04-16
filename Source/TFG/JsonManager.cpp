// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonManager.h"

#include "FileManager.h"
#include "Serialization/JsonSerializer.h"

/**
 * Metodo privado estatico que verifica que la extension para el archivo es correcta (es '.json') y la anade
 * en caso de que este ausente
 * 
 * @param JsonPath Ruta del archivo Json
 * @return Ruta comprobada del archivo Json
 */
FString UJsonManager::CheckJsonExtension(const FString& JsonPath)
{
	TArray<FString> PathArray;
	JsonPath.ParseIntoArray(PathArray, TEXT("."), true);

	if (PathArray.Top().ToLower() != "json") PathArray.Add("json");
	else PathArray[PathArray.Num()-1] = PathArray.Top().ToLower();

	return FString::Join(PathArray, TEXT("."));
}

/**
 * Metodo privado estatico que verifica que la ruta para el archivo es correcta tomando la ultima cadena de la ruta
 * dada y anadiendo al comienzo la ruta para almacenar archivos de guardado de mapas
 * 
 * @param JsonPath Ruta del archivo Json
 * @return Ruta comprobada del archivo Json
 */
FString UJsonManager::CheckMapPath(const FString& JsonPath)
{
	TArray<FString> SavePathArray;
	MapSavePath.ParseIntoArray(SavePathArray, TEXT("/"), true);
	
	TArray<FString> PathArray;
	JsonPath.ParseIntoArray(PathArray, TEXT("/"), true);

	FString ResultPath = MapSavePath;
	ResultPath += "/" + CheckJsonExtension(PathArray.Top());

	return ResultPath;
}

//--------------------------------------------------------------------------------------------------------------------//

/**
 * Metodo privado estatico que abre un archivo Json, lo lee y lo convierte a un JsonObject para ser procesado
 * 
 * @param JsonPath Ruta del archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 * @return El JsonObject con el contenido del archivo
 */
TSharedPtr<FJsonObject> UJsonManager::ReadJson(const FString& JsonPath, bool& Success, FString& ResultMessage)
{
	// Se intenta leer el archivo dado
	const FString JsonString = UFileManager::ReadStringFromFile(JsonPath, Success, ResultMessage);
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
void UJsonManager::WriteJson(const FString& JsonPath, const TSharedPtr<FJsonObject>& JsonObject, bool& Success, FString& ResultMessage)
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
 * @return Lista que contiene la informacion del archivo Json
 */
TArray<FMapData> UJsonManager::JsonToMapStruct(const FString JsonPath, bool& Success, FString& ResultMessage)
{
	TArray<FMapData> JsonData;
	
	// Se intenta crear un JsonObject de un archivo
	const TSharedPtr<FJsonObject> JsonObject = ReadJson(JsonPath, Success, ResultMessage);
	if (!Success) return JsonData;
	
	// Se intenta convertir el JsonObject en la estructura creada
	const TArray<TSharedPtr<FJsonValue>>* JsonDataArray;
	if (!JsonObject->TryGetArrayField(TEXT("TilesInfo"), JsonDataArray))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al convertir el objeto Json a la estructura interna"), *JsonPath);
		return JsonData;
	}

	for (int32 i = 0; i < JsonDataArray->Num(); ++i)
	{
		TSharedPtr<FJsonObject> JsonDataEntry = JsonDataArray->operator[](i)->AsObject();
		if (!JsonDataEntry.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("%s"), *FString::Printf(TEXT("(%s) [%d] ERROR: el elemento del array Json no es un objeto valido"), *JsonPath, i));
			continue;
		}

		FMapData DataEntry;
		DataEntry.Row = JsonDataEntry->GetIntegerField(TEXT("Row"));
		DataEntry.Col = JsonDataEntry->GetIntegerField(TEXT("Column"));
		DataEntry.TileType = JsonDataEntry->GetIntegerField(TEXT("TileType"));

		JsonData[i] = DataEntry;
	}

	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Conversion a la estructura interna correcta"), *JsonPath);
	return JsonData;
}

/**
 * Metodo estatico que transforma la estructura con la informacion a almacenar en un JsonObject
 * 
 * @param JsonPath Ruta del archivo Json
 * @param JsonData Lista que contiene la informacion del archivo Json
 * @param Success Resultado de la operacion
 * @param ResultMessage Informacion de la operacion
 */
void UJsonManager::MapStructToJson(const FString JsonPath, const TArray<FMapData>& JsonData, bool& Success, FString& ResultMessage)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> DataArray;
	for (const FMapData Data : JsonData)
	{
		TSharedPtr<FJsonObject> DataJsonObject = MakeShareable(new FJsonObject);
		DataJsonObject->SetNumberField(TEXT("Row"), Data.Row);
		DataJsonObject->SetNumberField(TEXT("Column"), Data.Col);
		DataJsonObject->SetNumberField(TEXT("TileType"), Data.TileType);

		// Agregar el objeto JSON del elemento al array JSON
		DataArray.Add(MakeShareable(new FJsonValueObject(DataJsonObject)));
	}

	JsonObject->SetArrayField(TEXT("TilesInfo"), DataArray);
	
	// Se intenta escribir los datos al archivo
	WriteJson(CheckMapPath(JsonPath), JsonObject, Success, ResultMessage);
}

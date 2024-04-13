// Fill out your copyright notice in the Description page of Project Settings.


#include "FileManager.h"

FString UFileManager::ReadStringFromFile(FString FilePath, bool& Success, FString& ResultMessage)
{
	// Se comprueba si el archivo con la ruta dada existe
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al leer el archivo. El archivo no existe"), *FilePath);
		return "";
	}

	FString ReadString = "";

	// Se intenta cargar el contenido del archivo al String resultado
	if (!FFileHelper::LoadFileToString(ReadString, *FilePath))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al leer el archivo. No se pudo leer, ¿es un archivo de texto?"), *FilePath);
		return "";		
	}

	// Se actualizan las variables sobre el estado de la operacion y se devuelve la cadena leida
	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Lectura correcta"), *FilePath);
	return ReadString;
}

void UFileManager::WriteStringToFile(FString FilePath, FString String, bool& Success, FString& ResultMessage)
{
	// Se intenta escribir la cadena dada en el archivo con la ruta especificada
	// Por defecto, se sobreescribe si el archivo existe o se crea en caso contrario
	if (!FFileHelper::SaveStringToFile(String, *FilePath))
	{
		Success = false;
		ResultMessage = FString::Printf(TEXT("(%s) ERROR: fallo al escribir el archivo"), *FilePath);
		return;
	}

	// Se actualizan las variables sobre el estado de la operacion
	Success = true;
	ResultMessage = FString::Printf(TEXT("(%s) Escritura correcta"), *FilePath);
}

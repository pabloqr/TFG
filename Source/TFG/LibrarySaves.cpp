// Fill out your copyright notice in the Description page of Project Settings.


#include "LibrarySaves.h"

#include "Kismet/GameplayStatics.h"

FString ULibrarySaves::FormatNumber(const int32 Number, const int32 DesiredLen)
{
	FString FormattedNumber = FString::FromInt(Number);
	for (int32 i = FormattedNumber.Len(); i < DesiredLen; ++i) FormattedNumber.InsertAt(0, TEXT("0"));

	return FormattedNumber;
}

//--------------------------------------------------------------------------------------------------------------------//

FString ULibrarySaves::GetSaveName(const ESaveType SaveType)
{
	// Se obtiene la fecha y hora actual
	const FDateTime CurrentTime = FDateTime::Now();

	// Se construyen los diferentes elementos del nombre
	const FString Prefix = SaveType == ESaveType::MapSave
		                       ? TEXT("MapSave")
		                       : SaveType == ESaveType::GameSave
		                       ? TEXT("GameSave")
		                       : TEXT("");

	const FString Time = FormatNumber(CurrentTime.GetHour(), 2) + FormatNumber(CurrentTime.GetMinute(), 2) +
		FormatNumber(CurrentTime.GetSecond(), 2);
	const FString Date = FormatNumber(CurrentTime.GetDay(), 2) + FormatNumber(CurrentTime.GetMonth(), 2) +
		FormatNumber(CurrentTime.GetYear(), 4);

	// Se concatenan separados por '_'
	return FString::Join(TArray<FString>({Prefix, Time, Date}), TEXT("_"));
}

//--------------------------------------------------------------------------------------------------------------------//

TArray<FSaveData> ULibrarySaves::GetSavesList(const ESaveType SaveType)
{
	// Se crea la variable que contendra las entradas de los diferentes archivos de guardado
	TArray<FSaveData> SavesList = TArray<FSaveData>();

	// Se intenta obtener el archivo de guardado 'master'
	if (const USaveList* SaveListInstance = Cast<USaveList>(UGameplayStatics::LoadGameFromSlot(TEXT("SavesList"), 0)))
	{
		// Se actualiza la lista de entradas
		if (SaveType == ESaveType::MapSave) SavesList.Append(SaveListInstance->MapSaves);
		else if (SaveType == ESaveType::GameSave) SavesList.Append(SaveListInstance->GameSaves);
	}

	return SavesList;
}

void ULibrarySaves::UpdateSaveList(const FString SaveName, const ESaveType SaveType, const FString CustomName)
{
	// Se obtienen las listas de archivos de guardado almacenados
	TArray<FSaveData> MapSavesList = GetSavesList(ESaveType::MapSave);
	TArray<FSaveData> GameSavesList = GetSavesList(ESaveType::GameSave);

	// Se elige la lista a actualizar segun el tipo de archivo de guardado que se quiere crear
	TArray<FSaveData>& SavesList = SaveType == ESaveType::MapSave ? MapSavesList : GameSavesList;

	// Se recorre la lista de archivos de guardado para verificar si el archivo dado existe o se debe crear
	int32 Index = -1;
	for (int32 i = 0; i < SavesList.Num() && Index == -1; ++i)
	{
		// Si el nombre coincide, se actualiza la lista con los nuevos datos
		if (SavesList[i].SaveName == SaveName)
		{
			// Se actualiza la fecha y hora
			SavesList[Index].SaveDate = FDateTime::Now();

			// Se actualiza el indice y se finaliza el bucle
			Index = i;
		}
	}

	// Si no se ha encontrado el archivo de guardado, se crea la nueva entrada
	if (Index == -1) SavesList.Add(FSaveData(SaveName, FDateTime::Now(), CustomName));

	// Se crea el nuevo archivo de guardado 'master' con los datos actualizados
	if (USaveList* SaveListInstance = Cast<USaveList>(
		UGameplayStatics::CreateSaveGameObject(USaveList::StaticClass())))
	{
		// Se actualizan las listas del archivo de guardado
		SaveListInstance->MapSaves = MapSavesList;
		SaveListInstance->GameSaves = GameSavesList;

		// Se almacena en el equipo el archivo de guardado
		if (!UGameplayStatics::SaveGameToSlot(SaveListInstance, TEXT("SavesList"), 0))
		{
			UE_LOG(LogTemp, Error, TEXT("ERROR: fallo al intentar guardar el archivo 'master'"))
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Guardado correcto"))
	}
}

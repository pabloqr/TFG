// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveList.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LibrarySaves.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API ULibrarySaves : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FString FormatNumber(const int32 Number, const int32 DesiredLen);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	static FString GetSaveName(const ESaveType SaveType);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	static TArray<FSaveData> GetSavesList(const ESaveType SaveType);

	UFUNCTION(BlueprintCallable)
	static void UpdateSaveList(const bool CreateSave, const FString SaveName, const ESaveType SaveType,
	                           const FString CustomName);

	UFUNCTION(BlueprintCallable)
	static void ClearSaveList(const ESaveType SaveType);
};

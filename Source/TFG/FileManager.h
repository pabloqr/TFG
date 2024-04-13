// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileManager.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API UFileManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FString ReadStringFromFile(FString FilePath, bool &Success, FString &ResultMessage);
	static void WriteStringToFile(FString FilePath, FString String, bool &Success, FString &ResultMessage);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SEditor.generated.h"

enum class ETileType : uint8;
/**
 * 
 */
UCLASS()
class TFG_API ASEditor : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="EditorState")
	FString CurrentMapName;
};

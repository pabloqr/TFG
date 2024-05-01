// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GInstance.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API UGInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameInstance|Grid")
	FVector2D GridSize;

	virtual void Init() override;
};

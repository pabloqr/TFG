// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGameInterface.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class TFG_API UWGameInterface : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* NextTurnButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void NextTurn();
};

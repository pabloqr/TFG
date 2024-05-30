// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDMainMode.generated.h"

/**
 * 
 */
UCLASS()
class TFG_API AHUDMainMode : public AHUD
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void ShowMenu();
	UFUNCTION(BlueprintCallable)
	void HideMenu();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> GameInterfaceClass;

	UPROPERTY()
	UUserWidget* GameInterface;
};

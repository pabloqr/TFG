// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WMainMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class TFG_API UWMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta=(BindWidget))
	UButton* LoadGameButton;

	UPROPERTY(meta=(BindWidget))
	UButton* WorldEditorButton;

	UPROPERTY(meta=(BindWidget))
	UButton* QuitButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartNewGame();
	UFUNCTION()
	void QuitGame();
};

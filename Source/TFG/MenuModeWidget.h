// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuModeWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class TFG_API UMenuModeWidget : public UUserWidget
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

	void NativeConstruct() override;

	UFUNCTION()
	void StartNewGame();
	UFUNCTION()
	void QuitGame();
};

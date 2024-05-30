// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDMainMode.h"

#include "Blueprint/UserWidget.h"

void AHUDMainMode::ShowMenu()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	GameInterface = CreateWidget<UUserWidget>(PlayerController, GameInterfaceClass);

	GameInterface->AddToViewport();
}

void AHUDMainMode::HideMenu()
{
	if (GameInterface)
	{
		GameInterface->RemoveFromViewport();
		GameInterface = nullptr;
	}
}
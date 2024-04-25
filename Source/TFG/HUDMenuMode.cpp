// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDMenuMode.h"

#include "Blueprint/UserWidget.h"

void AHUDMenuMode::ShowMenu()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	MainMenu = CreateWidget<UUserWidget>(PlayerController, MenuClass);

	MainMenu->AddToViewport();
}

void AHUDMenuMode::HideMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromViewport();
		MainMenu = nullptr;
	}
}

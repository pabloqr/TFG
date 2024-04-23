// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuModeHUD.h"

#include "Blueprint/UserWidget.h"

void AMenuModeHUD::ShowMenu()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	MainMenu = CreateWidget<UUserWidget>(PlayerController, MenuClass);

	MainMenu->AddToViewport();
}

void AMenuModeHUD::HideMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromViewport();
		MainMenu = nullptr;
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "WMainMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	NewGameButton->OnClicked.AddUniqueDynamic(this, &UWMainMenu::StartNewGame);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UWMainMenu::QuitGame);
}

//--------------------------------------------------------------------------------------------------------------------//

void UWMainMenu::StartNewGame()
{
	UGameplayStatics::OpenLevel(this, TEXT("GameLevel"));
}

void UWMainMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

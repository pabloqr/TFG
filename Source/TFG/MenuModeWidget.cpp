// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuModeWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMenuModeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NewGameButton->OnClicked.AddUniqueDynamic(this, &UMenuModeWidget::StartNewGame);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UMenuModeWidget::QuitGame);
}

//--------------------------------------------------------------------------------------------------------------------//

void UMenuModeWidget::StartNewGame()
{
	UGameplayStatics::OpenLevel(this, TEXT("HexWorld"));
}

void UMenuModeWidget::QuitGame()
{
}

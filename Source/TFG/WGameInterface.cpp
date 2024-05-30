// Fill out your copyright notice in the Description page of Project Settings.


#include "WGameInterface.h"

#include "PawnFaction.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWGameInterface::NativeConstruct()
{
	Super::NativeConstruct();

	NextTurnButton->OnClicked.AddUniqueDynamic(this, &UWGameInterface::NextTurn);
}

void UWGameInterface::NextTurn()
{
	UE_LOG(LogTemp, Log, TEXT("Next Turn"))

	APawnFaction* CurrentFaction = Cast<APawnFaction>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	CurrentFaction->TurnEnded();
	CurrentFaction->TurnStarted();
}

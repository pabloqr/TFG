// Fill out your copyright notice in the Description page of Project Settings.


#include "CMain.h"

#include "Kismet/GameplayStatics.h"

void ACMain::ProposeDeal(const FDealInfo& Deal) const
{
	UE_LOG(LogTemp, Log, TEXT("CMain - Deal proposed"));
	
	// Se llama al evento para gestionar la proposicion del trato
	OnDealProposed.Broadcast(Deal);
}

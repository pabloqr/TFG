// Fill out your copyright notice in the Description page of Project Settings.


#include "MMain.h"

#include "GInstance.h"
#include "PawnFaction.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"

void AMMain::NextTurn() const
{
	// Se actualiza la faccion actual
	if (ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		// Se actualiza el indice de la faccion actual
		const int32 NumFaction = (State->GetCurrentFaction() + 1) % State->GetNumFactions();
		State->SetCurrentFaction(NumFaction);

		// Se inicia el turno de la faccion actual
		if (APawnFaction* CurrentFaction = Cast<APawnFaction>(UGameplayStatics::GetPlayerPawn(GetWorld(), NumFaction)))
		{
			CurrentFaction->TurnStarted();
		}

		// Se actualiza el turno
		State->AddTurn();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AMMain::BeginPlay()
{
	Super::BeginPlay();

	// Se inicializan los parametros de la partida
	MaxNumFactions = 8;
	MaxTurns = 200;

	// Se inicializa el numero de facciones en juego
	if (ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		// Se obtiene la instancia del juego para inicializar el numero de facciones
		if (const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			State->SetNumFactions(GameInstance->NumFactions);
			State->SetCurrentFaction(GameInstance->NumFactions - 1);
		}

		// Se crea el array de facciones en juego con las indices dado su numero
		TArray<int32> Factions = TArray<int32>();
		for (int32 i = 0; i < State->GetNumFactions(); ++i) Factions.Add(i);

		// Se establece en el array de facciones en juego
		State->SetFactionsAlive(Factions);
	}
}

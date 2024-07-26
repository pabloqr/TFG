// Fill out your copyright notice in the Description page of Project Settings.


#include "MMain.h"

#include "CMainAI.h"
#include "GInstance.h"
#include "PawnFaction.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"

void AMMain::NextTurn() const
{
	// Se da paso al turno de la siguiente faccion
	if (ASMain* State = Cast<ASMain>(GameState))
	{
		// Se obtiene la faccion
		if (APawnFaction* CurrentFaction = State->NextFaction())
		{
			// Se inicia el turno de la faccion actual
			CurrentFaction->TurnStarted();

			// Si es un agente, se procesa el turno internamente
			if (ACMainAI* AIController = Cast<ACMainAI>(CurrentFaction->GetController()))
			{
				AIController->TurnStarted();
			}
		}
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
	if (ASMain* State = Cast<ASMain>(GameState))
	{
		// Se obtiene la instancia del juego para inicializar el numero de facciones
		if (const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			State->SetNumFactions(FMath::Clamp(GameInstance->NumFactions, 1, MaxNumFactions));
		}
	}

	// Se llama al evento tras la inicializacion de los parametros
	OnInitFinished.Broadcast();
}

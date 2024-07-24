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
			// Se actualiza el turno si es la primera faccion
			if (State->GetCurrentFaction() == 0) State->AddTurn();

			UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) NextFaction - %d"), State->GetCurrentTurn(), State->GetCurrentFaction()))

			// Se inicia el turno de la faccion actual
			CurrentFaction->TurnStarted();

			// Si es un agente, se procesa el turno internamente
			if (ACMainAI* AIController = Cast<ACMainAI>(CurrentFaction->GetController()))
			{
				UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("Is AI")))

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
			State->SetCurrentFaction(GameInstance->NumFactions - 1);
		}

		// Se crea el array de facciones en juego con las indices dado su numero
		TArray<int32> Factions = TArray<int32>();
		for (int32 i = 0; i < State->GetNumFactions(); ++i) Factions.Add(i);

		// Se establece en el array de facciones en juego
		State->SetFactionsAlive(Factions);
	}

	// Se llama al evento tras la inicializacion de los parametros
	OnInitFinished.Broadcast();
}

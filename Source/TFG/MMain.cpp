// Fill out your copyright notice in the Description page of Project Settings.


#include "MMain.h"

#include "CMainAI.h"
#include "GInstance.h"
#include "PawnFaction.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"

TMap<int32, float> AMMain::GetFactionsMilitaryStrength() const
{
	TMap<int32, float> FactionsMilitaryStrength = TMap<int32, float>();

	// Se verifica que la instancia del estado sea valida
	if (!State) return FactionsMilitaryStrength;

	// Se obtienen las facciones en juego y el diccionario de facciones y se procesan
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	for (const auto Faction : Factions)
	{
		// Si la faccion no sigue en juego, se omite
		if (!FactionsAlive.Contains(Faction.Key)) continue;

		// Se anade la entrada correspondiente de la faccion
		FactionsMilitaryStrength.Add(Faction.Key, Faction.Value->GetMilitaryStrength());
	}

	return FactionsMilitaryStrength;
}

//--------------------------------------------------------------------------------------------------------------------//

void AMMain::DeclareWarOnFaction(const int32 CurrentFaction, const int32 TargetFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(CurrentFaction) && FactionsAlive.Contains(TargetFaction) &&
		Factions.Contains(CurrentFaction) && Factions.Contains(TargetFaction))
	{
		Factions[CurrentFaction]->DeclareWarOnFaction(TargetFaction);
		Factions[TargetFaction]->DeclareWarOnFaction(CurrentFaction);
	}
}

float AMMain::GetWarScore(const int32 CurrentFaction, const int32 TargetFaction) const
{
	return State ? State->GetWarScore(CurrentFaction, TargetFaction) : -1.0;
}

float AMMain::GetWarTurns(const int32 CurrentFaction, const int32 TargetFaction) const
{
	return State ? State->GetWarTurns(CurrentFaction, TargetFaction) : -1;
}

float AMMain::ProposePeaceTreaty(const int32 TargetFaction, const FDealInfo& Deal) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return -1.0;

	// Se obtienen las facciones en juego y se verifica que la dada sea valida
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	if (!FactionsAlive.Contains(TargetFaction)) return -1.0;

	// Se obtienen todas las facciones y se verifica que ladada sea valida
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (!Factions.Contains(TargetFaction)) return -1.0;

	// Se obtiene el controlador de la faccion y se trata de hacer un cast al de la IA (o al del humano)
	const ACMainAI* FactionController = Cast<ACMainAI>(Factions[TargetFaction]->GetController());

	return FactionController ? FactionController->ProposePeaceTreaty(Deal) : -1.0;
}

void AMMain::MakePeaceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(CurrentFaction) && FactionsAlive.Contains(TargetFaction) &&
		Factions.Contains(CurrentFaction) && Factions.Contains(TargetFaction))
	{
		Factions[CurrentFaction]->MakePeaceWithFaction(TargetFaction);
		Factions[TargetFaction]->MakePeaceWithFaction(CurrentFaction);
	}
}

void AMMain::MakeAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(CurrentFaction) && FactionsAlive.Contains(TargetFaction) &&
		Factions.Contains(CurrentFaction) && Factions.Contains(TargetFaction))
	{
		Factions[CurrentFaction]->MakeAllianceWithFaction(TargetFaction);
		Factions[TargetFaction]->MakeAllianceWithFaction(CurrentFaction);
	}
}

void AMMain::BreakAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(CurrentFaction) && FactionsAlive.Contains(TargetFaction) &&
		Factions.Contains(CurrentFaction) && Factions.Contains(TargetFaction))
	{
		Factions[CurrentFaction]->BreakAllianceWithFaction(TargetFaction);
		Factions[TargetFaction]->BreakAllianceWithFaction(CurrentFaction);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AMMain::NextTurn() const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtiene la faccion
	if (APawnFaction* CurrentFaction = State->NextFaction())
	{
		// Antes de iniciar el turno, se actualiza la informacion sobre las facciones conocidas por la actual
		CurrentFaction->UpdateKnownFactionsInfo(GetFactionsMilitaryStrength());

		// Se inicia el turno de la faccion actual
		CurrentFaction->TurnStarted();

		// Si es un agente, se procesa el turno internamente
		if (ACMainAI* AIController = Cast<ACMainAI>(CurrentFaction->GetController()))
		{
			AIController->TurnStarted();
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

	// Se inicializa el estado del juego
	State = Cast<ASMain>(GameState);

	// Se inicializa el numero de facciones en juego
	if (State)
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

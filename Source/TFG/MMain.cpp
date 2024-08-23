// Fill out your copyright notice in the Description page of Project Settings.


#include "MMain.h"

#include "CMainAI.h"
#include "GInstance.h"
#include "PawnFaction.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"

void AMMain::MakePeaceWithFaction(const FDealInfo& Deal) const
{
	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	Factions[Deal.FactionAElements.FactionIndex]->MakePeaceWithFaction(Deal.FactionBElements.FactionIndex);
	Factions[Deal.FactionBElements.FactionIndex]->MakePeaceWithFaction(Deal.FactionAElements.FactionIndex);
}

void AMMain::MakeAllianceWithFaction(const FDealInfo& Deal) const
{
	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	Factions[Deal.FactionAElements.FactionIndex]->MakeAllianceWithFaction(Deal.FactionBElements.FactionIndex);
	Factions[Deal.FactionBElements.FactionIndex]->MakeAllianceWithFaction(Deal.FactionAElements.FactionIndex);
}

void AMMain::MakeExchangeDeal(const FDealInfo& Deal) const
{
	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	// Se actualiza el dinero de la faccion actual
	Factions[Deal.FactionAElements.FactionIndex]->AddMoney(Deal.FactionBElements.Money);
	Factions[Deal.FactionAElements.FactionIndex]->RemoveMoney(Deal.FactionAElements.Money);

	// Se actualiza el dinero de la faccion objetivo
	Factions[Deal.FactionBElements.FactionIndex]->AddMoney(Deal.FactionAElements.Money);
	Factions[Deal.FactionBElements.FactionIndex]->RemoveMoney(Deal.FactionBElements.Money);

	// Se actualizan los recursos de la faccion actual
	Factions[Deal.FactionAElements.FactionIndex]->AddResource(true, Deal.FactionBElements.Resource, -1);
	Factions[Deal.FactionAElements.FactionIndex]->RemoveResource(true, Deal.FactionAElements.Resource, -1);

	// Se actualizan los recursos de la faccion objetivo
	Factions[Deal.FactionBElements.FactionIndex]->AddResource(true, Deal.FactionAElements.Resource, -1);
	Factions[Deal.FactionBElements.FactionIndex]->RemoveResource(true, Deal.FactionBElements.Resource, -1);
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

//--------------------------------------------------------------------------------------------------------------------//

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

float AMMain::CalculateMoneyAmountForPeaceTreaty(const int32 TargetFaction, const bool ImLoosing,
                                                 const float WarScore, float StrengthDiffRel) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return 0.0;

	// Se verifica que la faccion es valida
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(TargetFaction) && Factions.Contains(TargetFaction))
	{
		return 0.0;
	}

	// Se obtiene la referencia a la faccion
	const APawnFaction* Faction = Factions[TargetFaction];

	// Se actualiza el valor de la comparativa de fuerza dependiendo de si es favorable o no
	StrengthDiffRel *= ImLoosing ? 1.0 : -1.0;

	// Se normaliza la puntuacion ente 0-1 y se introduce un tope de 1000 puntos
	const float NormWarScore = FMath::Min(WarScore, 1000.0f) / 1000.0;

	// Se aplica una funcion para regular la puntuacion de guerra
	const float WarScoreModifier = 1.0 / (1.0 + FMath::Exp(-10.0 * NormWarScore));

	// Se ajustan pesos en funcion del tanto por uno
	const float WarScoreWeight = !ImLoosing ? 0.5 : 0.55;
	const float StrengthDiffRelWeight = !ImLoosing ? 0.6 : 0.55;

	// Se combinan los valores de la puntuacion y la relevancia de la fuerza
	const float MoneyPercentage = WarScoreModifier * WarScoreWeight + StrengthDiffRel * StrengthDiffRelWeight;

	// Se calcula la cantidad de dinero y se devuelve
	const float MoneyAmount = FMath::Min(Faction->GetMoney() * MoneyPercentage, Faction->GetMoney());
	return FMath::Max(MoneyAmount, 0.0f);
}

//--------------------------------------------------------------------------------------------------------------------//

void AMMain::ProposeDeal(const FDealInfo& Deal) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen los indices de las facciones
	const int32 CurrentFaction = Deal.FactionAElements.FactionIndex;
	const int32 TargetFaction = Deal.FactionBElements.FactionIndex;

	// Se obtienen las facciones y se actualizan los estados de las dadas
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(CurrentFaction) && FactionsAlive.Contains(TargetFaction) &&
		Factions.Contains(CurrentFaction) && Factions.Contains(TargetFaction))
	{
		// Se obtiene la referencia para poder llamar al metodo correspondiente
		if (const IInterfaceDeal* InterfaceRef = Cast<IInterfaceDeal>(Factions[TargetFaction]->GetController()))
		{
			// Se obtiene la confirmacion de la faccion correspondiente para llevar a cabo el trato
			InterfaceRef->ProposeDeal(Deal);
		}
	}
}

void AMMain::ResolveDeal(const float DealResult, const FDealInfo& Deal) const
{
	// Si se ha aceptado el trato, se ejecuta la accion correspondiente dependiendo del tipo de trato
	if (DealResult > 0.0)
	{
		switch (Deal.Type)
		{
		case EDealType::WarDeal:
			MakePeaceWithFaction(Deal);
			break;
		case EDealType::AllianceDeal:
			MakeAllianceWithFaction(Deal);
			break;
		case EDealType::ExchangeDeal:
			MakeExchangeDeal(Deal);
			break;
		}
	}
}

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

void AMMain::BreakAllianceWithFaction(const int32 CurrentFaction, const int32 TargetFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se realiza el trato
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

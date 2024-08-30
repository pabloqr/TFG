// Fill out your copyright notice in the Description page of Project Settings.


#include "MMain.h"

#include "CMainAI.h"
#include "GInstance.h"
#include "PawnFaction.h"
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

TMap<int32, FWarInfo> AMMain::GetFactionsAtWarInfo() const
{
	TMap<int32, FWarInfo> FactionsAtWarInfo = TMap<int32, FWarInfo>();

	// Se verifica que la instancia del estado sea valida
	if (!State) return FactionsAtWarInfo;

	// Se obtiene el indice de la faccion actual
	const int32 CurrentIndex = State->GetCurrentFaction()->GetIndex();

	// Se obtienen las guerras para la faccion actual y se procesan
	const TMap<FFactionsPair, FWarInfo> CurrentWarsForFaction = State->GetCurrentWarsForFaction(CurrentIndex);
	for (const auto War : CurrentWarsForFaction)
	{
		// Se verifica que el indice de la faccion actual sea valido
		if (!War.Key.Contains(CurrentIndex)) continue;

		// Se actualiza el diccionario de facciones y guerras
		const int32 FactionIndex = War.Key.FactionA == CurrentIndex ? War.Key.FactionB : War.Key.FactionA;
		FactionsAtWarInfo.Add(FactionIndex, War.Value);
	}

	return FactionsAtWarInfo;
}

//--------------------------------------------------------------------------------------------------------------------//

float AMMain::CalculateMoneyAmountForPeaceTreaty(const int32 TargetFaction, const bool ImWeaker,
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
	StrengthDiffRel *= ImWeaker ? 1.0 : -1.0;

	// Se normaliza la puntuacion ente 0-1 y se introduce un tope de 1000 puntos
	const float NormWarScore = FMath::Min(WarScore, 1000.0f) / 1000.0;

	// Se aplica una funcion para regular la puntuacion de guerra
	const float WarScoreModifier = 1.0 / (1.0 + FMath::Exp(-10.0 * NormWarScore));

	// Se ajustan pesos en funcion del tanto por uno
	const float WarScoreWeight = !ImWeaker ? 0.5 : 0.55;
	const float StrengthDiffRelWeight = !ImWeaker ? 0.6 : 0.55;

	// Se combinan los valores de la puntuacion y la relevancia de la fuerza
	const float MoneyPercentage = WarScoreModifier * WarScoreWeight + StrengthDiffRel * StrengthDiffRelWeight;

	// Se calcula la cantidad de dinero y se devuelve
	return FMath::Clamp(Faction->GetMoney() * MoneyPercentage, 0.0f, Faction->GetMoney());
}

float AMMain::CalculateMoneyAmountForAllianceTreaty(const int32 TargetFaction, const float StrengthDiffRel) const
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

	return FMath::Clamp(Faction->GetMoney() * FMath::Pow(StrengthDiffRel, 2.0f), 0.0f, Faction->GetMoney());
}

//--------------------------------------------------------------------------------------------------------------------//

bool AMMain::IsFactionAtWarWith(const int32 TargetFaction, const int32 FactionAtWar) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return false;

	// Se obtienen las facciones y se procesa
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(TargetFaction) && FactionsAlive.Contains(FactionAtWar) &&
		Factions.Contains(TargetFaction) && Factions.Contains(FactionAtWar))
	{
		return Factions[TargetFaction]->GetFactionsAtWar().Contains(FactionAtWar);
	}

	return false;
}

bool AMMain::IsFactionNeutralWith(const int32 TargetFaction, const int32 NeutralFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return false;

	// Se obtienen las facciones y se procesa
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(TargetFaction) && FactionsAlive.Contains(NeutralFaction) &&
		Factions.Contains(TargetFaction) && Factions.Contains(NeutralFaction))
	{
		return Factions[TargetFaction]->GetFactionsAtWar().Contains(NeutralFaction);
	}

	return false;
}

bool AMMain::IsFactionAlliedWith(const int32 TargetFaction, const int32 AllyFaction) const
{
	// Se verifica que la instancia del estado sea valida
	if (!State) return false;

	// Se obtienen las facciones y se procesa
	const TSet<int32> FactionsAlive = State->GetFactionsAlive();
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (FactionsAlive.Contains(TargetFaction) && FactionsAlive.Contains(AllyFaction) &&
		Factions.Contains(TargetFaction) && Factions.Contains(AllyFaction))
	{
		return Factions[TargetFaction]->GetFactionsAtWar().Contains(AllyFaction);
	}

	return false;
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

	// Se verifica que la instancia del estado sea valida
	if (!State) return;

	// Se obtienen las facciones y se verifica que es valida
	const TMap<int32, APawnFaction*> Factions = State->GetFactions();
	if (!Factions.Contains(Deal.FactionAElements.FactionIndex)) return;

	// Se obtiene la referencia para poder llamar al metodo correspondiente
	if (ACMainAI* FactionController = Cast<ACMainAI>(Factions[Deal.FactionAElements.FactionIndex]->GetController()))
	{
		if (Deal.Type == EDealType::WarDeal) FactionController->ManageNextFactionAtWar();
		else if (Deal.Type == EDealType::AllianceDeal) FactionController->ManageNextNeutralFaction();
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
		// Se llama al evento para gestionar el mensaje a mostrar
		OnDiplomaticMessageSent.Broadcast(FDealInfo(
			EDealType::WarDeal, FDealElements(CurrentFaction, 0.0, FResource()),
			FDealElements(TargetFaction, 0.0, FResource())));

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
		// Se llama al evento para gestionar el mensaje a mostrar
		OnDiplomaticMessageSent.Broadcast(FDealInfo(
			EDealType::AllianceDeal, FDealElements(CurrentFaction, 0.0, FResource()),
			FDealElements(TargetFaction, 0.0, FResource())));

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
		CurrentFaction->UpdateKnownFactionsInfo(GetFactionsMilitaryStrength(), GetFactionsAtWarInfo());

		// Se inicia el turno de la faccion actual
		CurrentFaction->TurnStarted();

		// Si es un agente, se procesa el turno internamente
		if (ACMainAI* AIController = Cast<ACMainAI>(CurrentFaction->GetController()))
		{
			AIController->TurnStarted();
		}
	}
}

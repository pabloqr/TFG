// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorDamageableElement.h"

#include "SMain.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AActorDamageableElement::AActorDamageableElement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Se inicializan los atributos con los datos por defecto del asentamiento
	DamageableInfo = FDamageableInfo(-1, 400, FAttackStats(150.0, 200.0));
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::UpdateAttackAndDefenseParameters()
{
	const int32 HealthDamagePenalty = FMath::Floor(DamageableInfo.HealthPoints / 10.0) - 10;

	// Se actualizan los puntos de ataque y defensa
	DamageableInfo.Stats.AttackPoints = DamageableInfo.BaseStats.AttackPoints -
		DamageableInfo.BaseStats.AttackPoints * HealthDamagePenalty;
	DamageableInfo.Stats.DefensePoints = DamageableInfo.BaseStats.DefensePoints -
		DamageableInfo.BaseStats.DefensePoints * HealthDamagePenalty;
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorDamageableElement::IsMine() const
{
	// Se verifica si la unidad pertenece a la faccion en juego
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		return DamageableInfo.Owner == State->GetCurrentIndex();
	}

	return false;
}

bool AActorDamageableElement::IsEnemy() const
{
	// Se verifica si la unidad pertenece a la faccion en juego
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		const TMap<int32, APawnFaction*> Factions = State->GetFactions();
		if (Factions.Contains(State->GetCurrentIndex()))
		{
			return Factions[State->GetCurrentIndex()]->GetFactionsAtWar().Contains(DamageableInfo.Owner);
		}
	}

	return false;
}

bool AActorDamageableElement::IsNeutral() const
{
	// Se verifica si la unidad pertenece a la faccion en juego
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		const TMap<int32, APawnFaction*> Factions = State->GetFactions();
		if (Factions.Contains(State->GetCurrentIndex()))
		{
			return Factions[State->GetCurrentIndex()]->GetFactionsAtWar().Contains(DamageableInfo.Owner);
		}
	}

	return false;
}

bool AActorDamageableElement::IsAlly() const
{
	// Se verifica si la unidad pertenece a la faccion en juego
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		const TMap<int32, APawnFaction*> Factions = State->GetFactions();
		if (Factions.Contains(State->GetCurrentIndex()))
		{
			return Factions[State->GetCurrentIndex()]->GetFactionsAtWar().Contains(DamageableInfo.Owner);
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

float AActorDamageableElement::CalculateAttack(const bool IsAttacking, const FAttackStats& Stats) const
{
	// Se calculan y obtienen los puntos de fuerza del atacante y el defensor
	const float AttackStrength = DamageableInfo.Stats.GetStrengthPoints();
	const float DefenseStrength = Stats.GetStrengthPoints();

	// Se calcula la diferencia entre los puntos de fuerza
	const float StrengthDifference = IsAttacking ? DefenseStrength - AttackStrength : AttackStrength - DefenseStrength;

	// Se obtiene un valor que aleatorice ligeramente los parametros de ataque
	const float RandomModifier = FMath::RandRange(0.75f, 1.25f);

	return 30.0 * FMath::Exp(StrengthDifference / 25.0 * RandomModifier);
}

void AActorDamageableElement::PerformAttack(const bool IsAttacking, const FAttackStats& Stats)
{
	// Se calcula el dano a aplicar
	const float Damage = CalculateAttack(IsAttacking, Stats);

	// Se hace efectivo el dano
	ApplyDamage(Damage);

	// Se llama al evento para procesar el ataque
	OnAttackTriggered.Broadcast();
}

void AActorDamageableElement::ApplyDamage(const float Damage)
{
	// Se restan los puntos de vida correspondientes
	DamageableInfo.HealthPoints = FMath::Max(0.0f, DamageableInfo.HealthPoints - Damage);

	// Se actualizan los atributos de la unidad de acuerdo a la vida restante
	UpdateAttackAndDefenseParameters();
}

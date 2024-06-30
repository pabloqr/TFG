// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorDamageableElement.h"

#include "PawnFaction.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActorDamageableElement::AActorDamageableElement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DamageableInfo = FDamageableInfo();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::UpdateAttackAndDefenseParameters()
{
	const float HealthPercentage = 1.0 - DamageableInfo.HealthPoints / DamageableInfo.BaseHealthPoints;

	DamageableInfo.AttackPoints -= DamageableInfo.AttackPoints * HealthPercentage;
	DamageableInfo.DefensePoints -= DamageableInfo.DefensePoints * HealthPercentage;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------------------------------------------------------------------------------------------------//

bool AActorDamageableElement::IsMine() const
{
	// Se obtiene la faccion actual y se comprueba si el elemento pertenece a dicha faccion
	const APawnFaction* CurrentFaction = Cast<APawnFaction>(UGameplayStatics::GetPlayerPawn(GetWorld(), 1));
	if (CurrentFaction) return CurrentFaction->HasElement(this);
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

float AActorDamageableElement::CalculateAttack(const bool IsAttacking, const FAttackStats& Stats) const
{
	// Se obtiene un valor que aleatorice ligeramente los parametros de ataque
	const float RandomModifier = FMath::RandRange(-0.5f, 0.5f);

	// Se calculan las nuevas estadisticas
	const float ModAttackPoints = DamageableInfo.AttackPoints + DamageableInfo.AttackPoints * RandomModifier;
	const float ModDefensePoints = DamageableInfo.DefensePoints + DamageableInfo.DefensePoints * RandomModifier;
	
	// No cambiar el orden, esta bien se mire como se mire
	return IsAttacking ? Stats.AttackPoints - ModDefensePoints * 0.5 : ModAttackPoints - Stats.DefensePoints * 0.5;
}

void AActorDamageableElement::PerformAttack(const bool IsAttacking, const FAttackStats& Stats)
{
	// Se calcula el dano a aplicar y se hace efectivo
	const float Damage = CalculateAttack(IsAttacking, Stats);
	ApplyDamage(Damage);
}

void AActorDamageableElement::ApplyDamage(const float Damage)
{
	// Se restan los puntos de vida correspondientes
	DamageableInfo.HealthPoints = FMath::Clamp(DamageableInfo.HealthPoints - Damage, 0.0f, DamageableInfo.BaseHealthPoints);

	// Se actualizan los atributos de la unidad de acuerdo a la vida restante
	UpdateAttackAndDefenseParameters();
}

void AActorDamageableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


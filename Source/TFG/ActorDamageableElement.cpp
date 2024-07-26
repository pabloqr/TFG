// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorDamageableElement.h"

#include "SMain.h"
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

	// Se actualizan los puntos de ataque y se verifica que no sean negativos
	DamageableInfo.Stats.AttackPoints -= DamageableInfo.Stats.AttackPoints * HealthPercentage;
	DamageableInfo.Stats.AttackPoints = DamageableInfo.Stats.AttackPoints <= 0.0
		                                    ? 2.0
		                                    : DamageableInfo.Stats.AttackPoints;

	// Se actualizan los puntos de defensa y se verifica que no sean negativos
	DamageableInfo.Stats.DefensePoints -= DamageableInfo.Stats.DefensePoints * HealthPercentage;
	DamageableInfo.Stats.DefensePoints = DamageableInfo.Stats.DefensePoints <= 0.0
		                                     ? 2.0
		                                     : DamageableInfo.Stats.DefensePoints;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::BeginPlay()
{
	Super::BeginPlay();
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

//--------------------------------------------------------------------------------------------------------------------//

float AActorDamageableElement::CalculateAttack(const bool IsAttacking, const FAttackStats& Stats) const
{
	// Se obtiene un valor que aleatorice ligeramente los parametros de ataque
	const float RandomModifier = FMath::RandRange(-0.5f, 0.5f);

	// Se calculan las nuevas estadisticas
	const float ModAttackPoints = DamageableInfo.Stats.AttackPoints +
		DamageableInfo.Stats.AttackPoints * RandomModifier;
	const float ModDefensePoints = DamageableInfo.Stats.DefensePoints +
		DamageableInfo.Stats.DefensePoints * RandomModifier;

	// No cambiar el orden, esta bien se mire como se mire
	const float Damage = IsAttacking
		                     ? Stats.AttackPoints - ModDefensePoints * 0.5
		                     : ModAttackPoints - Stats.DefensePoints * 0.5;

	// Dado que se puede obtener un valor negativo, se verifica y se aplica el valor correcto
	return Damage <= 0.0 ? 2.0 : Damage;
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

void AActorDamageableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

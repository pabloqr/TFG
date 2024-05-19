// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorDamageableElement.h"


// Sets default values
AActorDamageableElement::AActorDamageableElement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthPoints = 100.0;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::UpdateAttackAndDefenseParameters()
{
	const float HealthPercentage = 1.0 - HealthPoints / BaseHealthPoints;

	AttackPoints -= AttackPoints * HealthPercentage;
	DefensePoints -= DefensePoints * HealthPercentage;
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------------------------------------------------------------------------------------------------//

FVector2D AActorDamageableElement::CalculateAttack(AActorDamageableElement* DamageableElement) const
{
	const float SelfDamage = DamageableElement->AttackPoints - DefensePoints * 0.5;
	const float EnemyDamage = AttackPoints - DamageableElement->DefensePoints * 0.5;

	return FVector2D(SelfDamage, EnemyDamage);
}

void AActorDamageableElement::PerformAttack(AActorDamageableElement* DamageableElement)
{
	const FVector2D Damage = CalculateAttack(DamageableElement);

	ApplyDamage(Damage.X);
	DamageableElement->ApplyDamage(Damage.Y);
}

void AActorDamageableElement::ApplyDamage(const float Damage)
{
	HealthPoints -= Damage;

	UpdateAttackAndDefenseParameters();
}

void AActorDamageableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


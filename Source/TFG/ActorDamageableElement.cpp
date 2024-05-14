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

// Called when the game starts or when spawned
void AActorDamageableElement::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorDamageableElement::PerformAttack(AActorDamageableElement DamageableElement) const
{
}

void AActorDamageableElement::ApplyDamage(const float Damage)
{
}

// Called every frame
void AActorDamageableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


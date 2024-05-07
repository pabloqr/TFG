// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorUnit.h"


// Sets default values
AActorUnit::AActorUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

float AActorUnit::GetCurrentHealthPoints() const
{
	return 0.0;
}

void AActorUnit::ApplyDamage(const float Damage)
{
}

// Called when the game starts or when spawned
void AActorUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


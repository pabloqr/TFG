// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorPlaceableElement.h"


// Sets default values
AActorPlaceableElement::AActorPlaceableElement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActorPlaceableElement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorPlaceableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>

// Sets default values
ATile::ATile()
{
 	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);
}

//--------------------------------------------------------------------------------------------------------------------//

FIntPoint ATile::GetMapPosition() const
{
	return MapPosition;
}

void ATile::SetPosition(const FIntPoint& Position)
{
	this->MapPosition = Position;
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
// void ATile::BeginPlay()
// {
//	Super::BeginPlay();
// }

// Called every frame
// void ATile::Tick(float DeltaTime)
// {
//	Super::Tick(DeltaTime);
// }

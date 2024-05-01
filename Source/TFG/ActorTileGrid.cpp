// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorTileGrid.h"


// Sets default values
AActorTileGrid::AActorTileGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TileGrid"));
}

void AActorTileGrid::AddTileInstance(const FVector2D MapPos2D, FIntPoint Pos2D)
{
	RemoveTileInstance(Pos2D);

	InstancedStaticMeshComponent->AddInstance(FTransform(FVector(MapPos2D.X, MapPos2D.Y, 0.01)));
	Positions.Add(Pos2D);
}

void AActorTileGrid::RemoveTileInstance(FIntPoint Pos2D)
{
	if (Positions.Contains(Pos2D))
	{
		InstancedStaticMeshComponent->RemoveInstance(Positions.Find(Pos2D));
		Positions.Remove(Pos2D);
	}
}

void AActorTileGrid::ClearTileInstances()
{
	InstancedStaticMeshComponent->ClearInstances();
	Positions.Empty();
}

// Called when the game starts or when spawned
void AActorTileGrid::BeginPlay()
{
	Super::BeginPlay();
	
}


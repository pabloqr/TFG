// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorResource.h"


// Sets default values
AActorResource::AActorResource()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Info = FResourceInfo();
}

//----------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void AActorResource::BeginPlay()
{
	Super::BeginPlay();
}

//----------------------------------------------------------------------------------------------------------------//

bool AActorResource::IsGathered() const
{
	return Info.Owner != -1;
}

//----------------------------------------------------------------------------------------------------------------//

// Called every frame
void AActorResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

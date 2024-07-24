// Fill out your copyright notice in the Description page of Project Settings.


#include "CMainAI.h"

#include "MMain.h"
#include "SMain.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACMainAI::ACMainAI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//--------------------------------------------------------------------------------------------------------------------//

void ACMainAI::TurnStarted()
{
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) AI Turn Started"), State->GetCurrentFaction()))
	}
	
	// Se finaliza el turno actual
	TurnFinished();
}

void ACMainAI::TurnFinished()
{
	if (const ASMain* State = Cast<ASMain>(UGameplayStatics::GetGameState(GetWorld())))
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("(%d) AI Turn Finished"), State->GetCurrentFaction()))
	}

	if (const AMMain* MainMode = Cast<AMMain>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		MainMode->NextTurn();
	}
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void ACMainAI::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

// Called every frame
void ACMainAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSettlement.h"

#include "FProductionElement.h"
#include "GInstance.h"
#include "LibraryTileMap.h"
#include "Kismet/GameplayStatics.h"


AActorSettlement::AActorSettlement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Info = FSettlementInfo();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::SetInitialOwnedTiles()
{
	// Se obtiene el tamano del mapa para verificar las posiciones
	FIntPoint MapSize;
	
	const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		MapSize = GameInstance->Size2D;
	}
	
	Info.OwnedTiles = ULibraryTileMap::GetNeighbors(Info.Pos2D, MapSize);
	Info.OwnedTiles.AddUnique(Info.Pos2D);
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::AddToProductionQueue(const TSubclassOf<AActorUnit> UnitToProduce)
{
	// Se anade el elemento a la cola de produccion
	Info.ProductionQueue.Add(FProductionElement(UnitToProduce, EUnitType::None, 5));

	// Se actualiza el estado
	Info.State = ESettlementState::Producing;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, Info.State);
}

void AActorSettlement::RemoveFromProduction(const int32 Index)
{
	// Se verifica que el indice sea valido
	if (0 <= Index && Index < Info.ProductionQueue.Num())
	{
		// Se elimina el elemento de la lista
		Info.ProductionQueue.RemoveAt(Index);

		// Se actualiza el estado
		Info.State = Info.ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;
		
		// Se llama al evento tras actualizar el estado
		OnSettlementStateChanged.Broadcast(this, Info.State);
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::BeginPlay()
{
	Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::TurnStarted()
{
	// Se comprueba si hay elementos en produccion
	if (Info.ProductionQueue.Num() > 0)
	{
		// Se actualiza el coste de produccion
		Info.ProductionQueue[0].ProductionCost -= 1;
		
		// Se comprueba si se han completado todos los turnos para producir la unidad
		if (Info.ProductionQueue[0].ProductionCost <= 0)
		{
			// Se obtiene la unidad a generar
			const TSubclassOf<AActorUnit> UnitToSpawn = Info.ProductionQueue[0].Unit;
		
			// Se elimina la unidad de la cola de produccion
			Info.ProductionQueue.RemoveAt(0);

			// Se llama al evento para generar la unidad en el mapa
			OnUnitProduced.Broadcast(Info.Pos2D, UnitToSpawn);
		}
	}

	// Se actualiza el estado
	Info.State = Info.ProductionQueue.Num() > 0 ? ESettlementState::Producing : ESettlementState::SelectProduction;

	// Se llama al evento tras actualizar el estado
	OnSettlementStateChanged.Broadcast(this, Info.State);
}

void AActorSettlement::TurnEnded()
{
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorSettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorPlaceableElement.h"

#include "GInstance.h"
#include "LibraryTileMap.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActorPlaceableElement::AActorPlaceableElement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ElementIdentifier = FElementIdentifier();
}

//--------------------------------------------------------------------------------------------------------------------//

// Called when the game starts or when spawned
void AActorPlaceableElement::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------------------------------------------------------------------------------------------------//

void AActorPlaceableElement::SetPos(const FIntPoint& Pos, const FVector2D& MapPos)
{
	SetPos(Pos);
	SetMapPos(MapPos);
}

TArray<FIntPoint> AActorPlaceableElement::GetNeighbors() const
{
	// Se preasigna el desplazamiento en funcion de si la columna es par o impar
	TArray<FIntPoint> AllNeighbors = Pos2D.Y % 2 == 0 ? TArray<FIntPoint> {
		FIntPoint(-1, -1), FIntPoint(-1, 0), FIntPoint(-1, 1),
		FIntPoint(0, 1), FIntPoint(1, 0), FIntPoint(0, -1)
	}
	: TArray<FIntPoint> {
		FIntPoint(0, -1), FIntPoint(-1, 0), FIntPoint(0, 1),
		FIntPoint(1, 1), FIntPoint(1, 0), FIntPoint(1, -1)
	};

	// Se obtiene el tamano del mapa para verificar las posiciones
	FIntPoint MapSize;
	
	const UGInstance* GameInstance = Cast<UGInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		MapSize = GameInstance->Size2D;
	}
	
	// Se actualizan las posiciones de los vecinos y se comprueba si son correctas
	TArray<FIntPoint> Neighbors;
	for (int32 i = 0; i < AllNeighbors.Num(); ++i)
	{
		AllNeighbors[i] += Pos2D;
		if (ULibraryTileMap::CheckValidPosition(AllNeighbors[i], MapSize)) Neighbors.Add(AllNeighbors[i]);
	}
	
	return Neighbors;
}

int32 AActorPlaceableElement::GetDistanceToElement(const FIntPoint& Pos) const
{
	const FIntVector Distance = OffsetCoordsToCubeCoords(Pos) - OffsetCoordsToCubeCoords(Pos2D);
	return FMath::Max3(Distance.X, Distance.Y, Distance.Z);
}

// Called every frame
void AActorPlaceableElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//--------------------------------------------------------------------------------------------------------------------//

FIntVector AActorPlaceableElement::OffsetCoordsToCubeCoords(const FIntPoint& OffsetCoords)
{
	const int32 X = OffsetCoords.Y;
	const int32 Y = OffsetCoords.X - (OffsetCoords.Y - FMath::Abs(OffsetCoords.Y));
	return FIntVector(X, Y, -X-Y);
}

FIntPoint AActorPlaceableElement::CubeCoordsToOffsetCoords(const FIntVector& CubeCoords)
{
	return FIntPoint(CubeCoords.Y + (CubeCoords.X - FMath::Abs(CubeCoords.X) / 2), CubeCoords.X);
}

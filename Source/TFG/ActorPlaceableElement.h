// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FElementIdentifier.h"
#include "GameFramework/Actor.h"
#include "ActorPlaceableElement.generated.h"

UCLASS(Abstract)
class TFG_API AActorPlaceableElement : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement")
	FElementIdentifier ElementIdentifier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Position")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Position")
	FVector2D MapPos2D;
	
public:
	/**
	 * Constructor por defecto
	 */
	AActorPlaceableElement();

protected:
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	const FElementIdentifier& GetIdentifier() const { return ElementIdentifier; }
	const FIntPoint& GetPos() const { return Pos2D; }
	const FVector2D& GetMapPos() const { return MapPos2D; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetIdentifier(const FElementIdentifier& Identifier) { this->ElementIdentifier = Identifier; }
	void SetPos(const FIntPoint& Pos, const FVector2D& MapPos);
	void SetPos(const FIntPoint& Pos) { this->Pos2D = Pos; }
	void SetMapPos(const FVector2D& Pos) { this->MapPos2D = Pos; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetNeighbors() const;
	int32 GetDistanceToElement(const FIntPoint& Pos) const;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado en cada frame
	 * 
	 * @param DeltaTime Tiempo transcurrido desde el ultimo frame
	 */
	virtual void Tick(float DeltaTime) override;

private:
	static FIntVector OffsetCoordsToCubeCoords(const FIntPoint& OffsetCoords);
	static FIntPoint CubeCoordsToOffsetCoords(const FIntVector& CubeCoords);
};

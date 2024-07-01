// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FResourceInfo.h"
#include "ActorResource.generated.h"

UCLASS()
class TFG_API AActorResource : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	FResourceInfo Info;

public:
	// Sets default values for this actor's properties
	AActorResource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/**
	 * Getter del atributo Pos2D
	 * 
	 * @return Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	const FIntPoint& GetPos() const { return Info.Pos2D; }

	EResourceType GetType() const { return Info.Type; }
	int32 GetQuantity() const { return Info.Quantity; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter del atributo Pos2D
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	void SetPos(const FIntPoint& Pos) { Info.Pos2D = Pos; }

	//----------------------------------------------------------------------------------------------------------------//

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorResource.h"
#include "FElementIdentifier.h"
#include "GameFramework/Pawn.h"
#include "PawnFaction.generated.h"

class AActorDamageableElement;
class AActorUnit;
class AActorSettlement;

UCLASS()
class TFG_API APawnFaction : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction")
	FElementIdentifier ElementIdentifier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Stats")
	float Money;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Stats")
	float MoneyBalance;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<AActorSettlement*> Settlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<AActorUnit*> Units;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<AActorResource*> Resources;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<int32> IdleSettlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<int32> ManualUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Faction|Elements")
	TArray<int32> AutomaticUnits;

public:
	/**
	 * Constructor por defecto
	 */
	APawnFaction();

protected:
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	//----------------------------------------------------------------------------------------------------------------//

	bool HasElement(const AActorDamageableElement* Element) const;
	
	//----------------------------------------------------------------------------------------------------------------//
	
	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnEnded();
	
	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Metodo ejecutado en cada frame
	 * 
	 * @param DeltaTime Tiempo transcurrido desde el ultimo frame
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Metodo que asocia un metodo de entrada para la interaccion entre el jugador y el juego
	 * 
	 * @param PlayerInputComponent Entrada del jugador
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

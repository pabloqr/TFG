// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnFaction.generated.h"

class AActorSettlement;
enum class ESettlementState : uint8;
enum class EUnitState : uint8;
class AActorDamageableElement;
class AActorUnit;
class AActorResource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnStarted);

UCLASS()
class TFG_API APawnFaction : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Info")
	int32 Index;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float Money;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float MoneyBalance;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorSettlement*> Settlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorUnit*> Units;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorResource*> Resources;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> IdleSettlements;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> ManualUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> AutomaticUnits;

public:
	/**
	 * Constructor por defecto
	 */
	APawnFaction();

protected:
	UFUNCTION(BlueprintCallable, Category="Faction")
	void OnUnitStateUpdated(const AActorUnit* Unit, const EUnitState State);

	UFUNCTION(BlueprintCallable, Category="Faction")
	void OnSettlementStateUpdated(const AActorSettlement* Settlement, const ESettlementState State);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	//----------------------------------------------------------------------------------------------------------------//

	int32 GetIndex() const { return Index; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetIndex(const int32 I) { Index = I; }

	//----------------------------------------------------------------------------------------------------------------//

	bool HasElement(const AActorDamageableElement* Element) const;
	
	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddSettlement(AActorSettlement* Settlement);

	UFUNCTION(BlueprintCallable)
	void RemoveSettlement(AActorSettlement* Settlement);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void AddUnit(AActorUnit* Unit);

	UFUNCTION(BlueprintCallable)
	void RemoveUnit(AActorUnit* Unit);

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnEnded();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que asocia un metodo de entrada para la interaccion entre el jugador y el juego
	 * 
	 * @param PlayerInputComponent Entrada del jugador
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnTurnStarted OnTurnStarted;
};

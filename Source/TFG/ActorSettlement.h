// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "FProductionElement.h"
#include "ActorSettlement.generated.h"

class AActorUnit;

//--------------------------------------------------------------------------------------------------------------------//

UENUM(BlueprintType)
enum class ESettlementState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	SelectProduction = 1 UMETA(DisplayName="SelectProduction"),
	Producing = 2 UMETA(DisplayName="Producing"),
};

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitProduced, const FIntPoint&, Pos2D, TSubclassOf<AActorUnit>, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettlementStateChanged, const AActorSettlement*, Settlement, const ESettlementState&, State);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API AActorSettlement : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	ESettlementState State;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FIntPoint> OwnedTiles;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	TArray<FProductionElement> ProductionQueue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	float MoneyYield;

public:
	/**
	 * Constructor por defecto de la clase que inicializa los tributos
	 */
	AActorSettlement();

protected:
	/**
	 * Metodo que establece las casillas que posee inicialmente el asentamiento
	 */
	UFUNCTION(BlueprintCallable, Category="Settlement")
	void SetInitialOwnedTiles();

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="Settlement")
	void AddToProductionQueue(const TSubclassOf<AActorUnit> UnitToProduce);

	UFUNCTION(BlueprintCallable, Category="Settlement")
	void RemoveFromProduction(const int32 Index);

	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	ESettlementState GetState() const;
	float GetMoneyYield() const;
	
	void SetState(const ESettlementState SettlementState);
	void SetMoneyYield(const float Yield);

	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Metodo que actualiza todos los atributos del asentamiento al comienzo del turno
	 */
	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	/**
	 * Metodo que actualiza todos los atributos del asentamiento al final del turno
	 */
	UFUNCTION(BlueprintCallable)
	void TurnEnded();

	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Metodo ejecutado en cada frame
	 * 
	 * @param DeltaTime Tiempo transcurrido desde el ultimo frame
	 */
	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnUnitProduced OnUnitProduced;

	UPROPERTY(BlueprintAssignable)
	FOnSettlementStateChanged OnSettlementStateChanged;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "FProductionElement.h"
#include "FSettlementInfo.h"
#include "Engine/DataTable.h"
#include "ActorSettlement.generated.h"

class AActorUnit;

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileOwned, const FIntPoint&, Pos2D);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitProduced, const FIntPoint&, Pos2D, const FProductionElement&, Unit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettlementStateChanged, const AActorSettlement*, Settlement,
                                             ESettlementState, State);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS()
class TFG_API AActorSettlement : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Settlement")
	FSettlementInfo Info;

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
	void AddToProductionQueue(const UDataTable* DataTable, const TSubclassOf<AActorUnit> UnitClass,
	                          const EUnitType UnitType);

	UFUNCTION(BlueprintCallable, Category="Settlement")
	void RemoveFromProduction(const UDataTable* DataTable, const int32 Index);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Getter del atributo Pos2D
	 * 
	 * @return Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	const FIntPoint& GetPos() const { return Info.Pos2D; }

	/**
	 * Getter del atributo OwnedTiles
	 * 
	 * @return Posiciones de las casillas en posesion del asentamiento
	 */
	const TArray<FIntPoint>& GetOwnedTiles() const { return Info.OwnedTiles; }

	/**
	 * Getter del atributo State
	 * 
	 * @return Estado del asentamiento
	 */
	ESettlementState GetState() const { return Info.State; }

	/**
	 * Getter del atributo MoneyYield
	 * 
	 * @return Cantidad de dinero que genera por turno. Puede ser un valor negativo
	 */
	float GetMoneyYield() const { return Info.MoneyYield; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter del atributo Pos2D
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	void SetPos(const FIntPoint& Pos) { Info.Pos2D = Pos; }

	/**
	 * Setter del atributo State
	 * 
	 * @param SettlementState Estado del asentamiento
	 */
	void SetState(const ESettlementState SettlementState) { Info.State = SettlementState; }

	/**
	 * Setter del atributo MoneyYield
	 * 
	 * @param Yield Cantidad de dinero que genera por turno. Puede ser un valor negativo
	 */
	void SetMoneyYield(const float Yield) { Info.MoneyYield = Yield; }

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
	FOnTileOwned OnTileOwned;

	UPROPERTY(BlueprintAssignable)
	FOnUnitProduced OnUnitProduced;

	UPROPERTY(BlueprintAssignable)
	FOnSettlementStateChanged OnSettlementStateChanged;
};

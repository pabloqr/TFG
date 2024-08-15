// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "FMovement.h"
#include "FUnitInfo.h"
#include "GameFramework/Actor.h"
#include "ActorUnit.generated.h"

class AActorTileMap;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitMoved, const FIntPoint&, PrevPos);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitStateChanged, const AActorUnit*, Unit, const EUnitState, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDestroyed, AActorUnit*, Unit);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS(Abstract)
class TFG_API AActorUnit : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	FUnitInfo Info;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|AditionalData")
	AActorTileMap* TileMap;

public:
	/**
	 * Constructor por defecto de la clase que inicializa los atributos de la instancia
	 */
	AActorUnit();

private:
	/**
	 * Metodo privado que actualiza los costes de movimiento de las casillas del camino para reflejar la cantidad
	 * correcta de turnos en llegar a cada una de ellas
	 */
	void UpdatePathCosts();

	/**
	 * Metodo privado que actualiza el numero de turnos que se necesitaran para alcanzar cada una de las casillas
	 */
	void UpdatePathTurns();

protected:
	UFUNCTION(BlueprintCallable)
	const TArray<FMovement>& GetPathCompleted() const { return Info.PathCompleted; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que aplica el movimiento dado actualizando los puntos de movimiento y el estado
	 * 
	 * @param Move Movimiento a aplicar
	 */
	UFUNCTION(BlueprintCallable, Category="Unit|Movement")
	void UpdatePosition(const FMovement& Move);

public:
	/**
	 * Getter del atributo Info
	 * 
	 * @return Informacion de la unidad
	 */
	const FUnitInfo& GetInfo() const { return Info; }

	/**
	 * Getter del atributo Pos2D
	 * 
	 * @return Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	const FIntPoint& GetPos() const { return Info.Pos2D; }

	/**
	 * Getter del atributo Type
	 * 
	 * @return Tipo de unidad
	 */
	EUnitType GetType() const { return Info.Type; }

	/**
	 * Getter del atributo State
	 * 
	 * @return Estado de la unidad
	 */
	EUnitState GetState() const { return Info.State; }

	/**
	 * Getter del atributo MaintenanceCost
	 * 
	 * @return Coste de mantenimiento de la unidad
	 */
	float GetMaintenanceCost() const { return Info.MaintenanceCost; }

	/**
	 * Getter del atributo BaseMovementPoints
	 * 
	 * @return Puntos de movimiento base
	 */
	int32 GetBaseMovementPoints() const { return Info.BaseMovementPoints; }

	/**
	 * Getter del atributo MovementPoints
	 * 
	 * @return Puntos de movimiento actuales
	 */
	int32 GetMovementPoints() const { return Info.MovementPoints; }

	/**
	 * Getter del atributo Path
	 * 
	 * @return Camino que va a seguir la unidad
	 */
	UFUNCTION(BlueprintCallable)
	const TArray<FMovement>& GetPath() const { return Info.Path; }

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
	 * @param State Estado de la unidad
	 */
	void SetState(const EUnitState State);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que asigna un nuevo camino a la unidad
	 * 
	 * @param NewPath Camino que debe seguir la unidad
	 */
	UFUNCTION(BlueprintCallable)
	void AssignPath(const TArray<FMovement>& NewPath);

	/**
	 * Metodo que actualiza el camino actual
	 */
	UFUNCTION(BlueprintCallable)
	void UpdatePath();

	/**
	 * Metodo que elimina el camino que la unidad estaba siguiendo
	 */
	UFUNCTION(BlueprintCallable)
	void RemovePath();

	/**
	 * Metodo que aplica todos los movimientos posibles a la unidad tomando el camino asignado como referencia
	 */
	UFUNCTION(BlueprintCallable)
	void ContinuePath();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que hace que la unidad este inactiva hasta que se cure completamente
	 */
	UFUNCTION(BlueprintCallable)
	void Heal();

	/**
	 * Metodo que salta el turno de la unidad aunque le queden puntos de movimiento
	 */
	UFUNCTION(BlueprintCallable)
	void SkipTurn();

	/**
	 * Metodo que realiza, si es posible, el siguiente movimiento del camino asignado
	 * 
	 * @return Si se ha apilcado el movimiento
	 */
	UFUNCTION(BlueprintCallable)
	void MoveUnit();

	/**
	 * Metodo que restablece los puntos de movimiento de la unidad
	 */
	UFUNCTION(BlueprintCallable)
	void RestoreMovement();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que aplica un dano a la unidad
	 * 
	 * @param Damage Dano a aplicar
	 */
	virtual void ApplyDamage(const float Damage) override;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que actualiza todos los atributos de la unidad al comienzo del turno
	 * 
	 * @return Estado de la unidad
	 */
	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	/**
	 * Metodo que actualiza todos los atributos de la unidad al final del turno
	 */
	UFUNCTION(BlueprintCallable)
	void TurnEnded();

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintAssignable)
	FOnUnitMoved OnUnitMoved;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnUnitStateChanged OnUnitStateChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnUnitDestroyed OnUnitDestroyed;
};

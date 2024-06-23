// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "ActorTileMap.h"
#include "FMovement.h"
#include "GameFramework/Actor.h"
#include "ActorUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitMoved, const FIntPoint&, PrevPos, const TArray<FMovement>&, Moves);

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	WaitingForOrders = 1 UMETA(DisplayName="WaitingForOrders"),
	Sleeping = 2 UMETA(DisplayName="Sleeping"),
	FollowingPath = 3 UMETA(DisplayName="FollowingPath"),
	NoMovementPoints = 4 UMETA(DisplayName="NoMovementPoints"),
	WaitingForNextTurn = 5 UMETA(DisplayName="WaitingForNextTurn")
};

UCLASS(Abstract)
class TFG_API AActorUnit : public AActorDamageableElement
{
	GENERATED_BODY()

protected:
	/**
	 * Estado
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	EUnitState State;

	/**
	 * Puntos de movimiento base
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Unit")
	int32 BaseMovementPoints;
	/**
	 * Puntos de movimiento en un instante concreto
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	int32 MovementPoints;

	/**
	 * Camino a seguir
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Path")
	TArray<FMovement> Path;
	/**
	 * Casillas completadas del camino en el turno actual
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Path")
	TArray<FMovement> PathCompleted;

	/**
	 * Puntos de visibilidad
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	int32 VisibilityPoints;

	/**
	 * Coste de produccion
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	float ProductionCost;
	/**
	 * Coste de mantenimiento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit")
	float MaintenanceCost;

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
	/**
	 * Metodo que aplica el movimiento dado actualizando los puntos de movimiento y el estado
	 * 
	 * @param Move Movimiento a aplicar
	 */
	UFUNCTION(BlueprintCallable, Category="Unit|Movement")
	void UpdatePosition(const FMovement& Move);

	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Getter del atributo State
	 * 
	 * @return Estado de la unidad
	 */
	EUnitState GetState() const { return State; }
	/**
	 * Getter del atributo MaintenanceCost
	 * 
	 * @return Coste de mantenimiento de la unidad
	 */
	float GetMaintenanceCost() const { return MaintenanceCost; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que asigna un nuevo camino a la unidad
	 * 
	 * @param NewPath Camino que debe seguir la unidad
	 */
	UFUNCTION(BlueprintCallable)
	void AssignPath(const TArray<FMovement>& NewPath);

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
	 * Metodo que actualiza todos los atributos de la unidad al comienzo del turno
	 * 
	 * @return Estado de la unidad
	 */
	UFUNCTION(BlueprintCallable)
	EUnitState TurnStarted();

	/**
	 * Metodo que actualiza todos los atributos de la unidad al final del turno
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
	FOnUnitMoved OnUnitMoved;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTileInfo.h"
#include "GameFramework/Actor.h"
#include "ActorTile.generated.h"

class AActorDamageableElement;
class AActorSettlement;
class AActorUnit;
class AActorResource;

UCLASS()
class TFG_API AActorTile : public AActor
{
	GENERATED_BODY()

protected:
	/**
	 * Representacion visual de la casilla
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile")
	FTileInfo Info;

public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTile();

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Getter del atributo Pos2D
	 * 
	 * @return Posicion en el Array2D de casillas
	 */
	UFUNCTION(BlueprintCallable)
	const FIntPoint& GetPos() const { return Info.Pos2D; }

	/**
	 * Getter del atributo MapPos2D
	 * 
	 * @return Posicion en la escena de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	const FVector2D& GetMapPos() const { return Info.MapPos2D; }

	/**
	 * Getter del atributo Type
	 * 
	 * @return Tipo de casilla
	 */
	const ETileType& GetType() const { return Info.Type; }

	/**
	 * Metodo que devuelve el coste de paso por la casilla dado su tipo
	 * 
	 * @return Coste de paso por la casilla
	 */
	int32 GetMovementCost() const;

	/**
	 * Getter de los atributos Unit/Settlement
	 * 
	 * @return Elemento situado sobre la casilla
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Tile|Elements")
	const AActorDamageableElement* GetElement() const;

	/**
	 * Getter del atributo Resource
	 * 
	 * @return Recurso situado sobre la casilla
	 */
	UFUNCTION(BlueprintCallable, Category="Tile|Elements")
	const AActorResource* GetResource() const;

	/**
	 * Getter del atributo TileStatess
	 * 
	 * @return Estado de la casilla
	 */
	const TSet<ETileState>& GetState() const { return Info.States; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter de los atributos Pos2D y MapPos2D
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 * @param MapPos Posicion en la escena
	 */
	UFUNCTION(BlueprintCallable)
	void SetPos(const FIntPoint& Pos, const FVector2D& MapPos);

	/**
	 * Setter del atributo Pos2D
	 * 
	 * @param Pos Posicion en el Array2D de casillas
	 */
	void SetPos(const FIntPoint& Pos) { Info.Pos2D = Pos; }

	/**
	 * Setter del atributo MapPos2D
	 * 
	 * @param Pos Posicion en la escena de la casilla
	 */
	void SetMapPos(const FVector2D& Pos) { Info.MapPos2D = Pos; }

	/**
	 * Setter del atributo Owner
	 * 
	 * @param OwnerFaction Faccion propietaria del recurso
	 */
	UFUNCTION(BlueprintCallable)
	void SetFactionOwner(const int32 OwnerFaction) { Info.Owner = OwnerFaction; }

	/**
	 * Setter del atributo Type
	 * 
	 * @param TileType Tipo de casilla
	 */
	void SetType(const ETileType TileType);

	/**
	 * Setter del atributo Resource
	 * 
	 * @param Resource Recurso situado sobre la casilla
	 */
	void SetResource(AActorResource* Resource);

	/**
	 * Setter del atributo Unit
	 * 
	 * @param Unit Unidad situado sobre la casilla
	 */
	void SetUnit(AActorUnit* Unit);

	/**
	 * Setter del atributo Settlement
	 * 
	 * @param Settlement Asentamiento situado sobre la casilla
	 */
	void SetSettlement(AActorSettlement* Settlement);

	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param TileStates Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void SetState(const TSet<ETileState>& TileStates);
	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param State Estado de la casilla
	 */
	void SetState(const ETileState State);

	/**
	 * Anade los estados dados a los actuales
	 * 
	 * @param TileStates Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void AddState(const TSet<ETileState>& TileStates);
	/**
	 * Anade el estado dado a los actuales
	 * 
	 * @param State Estado de la casilla
	 */
	void AddState(const ETileState State);

	/**
	 * Elimina los estados dados de los actuales
	 * 
	 * @param TileStates Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveState(const TSet<ETileState>& TileStates);
	/**
	 * Elimina el estado dado de los actuales
	 * 
	 * @param State Estado de la casilla
	 */
	void RemoveState(const ETileState State);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que verifica si una casilla es accesible
	 * 
	 * @return Si la casilla es de un tipo accesible
	 */
	UFUNCTION(BlueprintCallable)
	bool IsAccesible() const;

	/**
	 * Metodo que verifica si la casilla esta ocupada
	 * 
	 * @return Si la casilla contiene un elemento del juego
	 */
	UFUNCTION(BlueprintCallable)
	bool HasElement() const;

	/**
	 * Metodo que verifica si la casilla ya contiene un recurso
	 * 
	 * @return Si la casilla contiene un recurso
	 */
	UFUNCTION(BlueprintCallable)
	bool HasResource() const;

	/**
	 * Metodo que verifica si la casilla ya contiene una unidad
	 * 
	 * @return Si la casilla esta ocupada por una unidad
	 */
	UFUNCTION(BlueprintCallable)
	bool HasUnit() const;

	/**
	 * Metodo que verifica si la casilla ya contiene un asentamiento
	 * 
	 * @return Si la casilla esta ocupada por un asentamiento
	 */
	UFUNCTION(BlueprintCallable)
	bool HasSettlement() const;
};

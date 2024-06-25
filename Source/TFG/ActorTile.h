// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPlaceableElement.h"
#include "GameFramework/Actor.h"
#include "ActorTile.generated.h"

class AActorDamageableElement;
class AActorSettlement;
class AActorUnit;
class AActorResource;

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Plains = 1 UMETA(DisplayName="Plains"),
	Hills = 2 UMETA(DisplayName="Hills"),
	Forest = 3 UMETA(DisplayName="Forest"),
	SnowPlains = 4 UMETA(DisplayName="SnowPlains"),
	SnowHills = 5 UMETA(DisplayName="SnowHills"),
	Ice = 6 UMETA(DisplayName="Ice"),
	Mountains = 7 UMETA(DisplayName="Mountains"),
	Water = 8 UMETA(DisplayName="Water"),
	Max = 255 UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETileState : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Hovered = 1 UMETA(DisplayName="Hovered"),
	HoveredWithFriendElement = 2 UMETA(DisplayName="HoveredWithFriendElement"),
	HoveredWithEnemyElement = 3 UMETA(DisplayName="HoveredWithEnemyElement"),
	Selected = 4 UMETA(DisplayName="Selected"),
	Owned = 5 UMETA(DisplayName="Owned"),
	Neighbor = 6 UMETA(DisplayName="Neighbor"),
	Reachable = 7 UMETA(DisplayName="Reachable"),
	InPath = 8 UMETA(DisplayName="InPath")
};

UCLASS()
class TFG_API AActorTile : public AActorPlaceableElement
{
	GENERATED_BODY()

protected:
	/**
	 * Representacion visual de la casilla
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Tile")
	UStaticMeshComponent* TileMesh;

	/**
	 * Tipo de casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	ETileType TileType;
	/**
	 * Coste de movimiento de la casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	int32 MovementCost;

	/**
	 * Recurso sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Elements")
	AActorResource* Resource;
	
	/**
	 * Unidad sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Elements")
	AActorUnit* Unit;
	/**
	 * Asentamiento sobre la casilla. Si no posee, se establece como null
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Elements")
	AActorSettlement* Settlement;

	/**
	 * Estados de la casilla
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Tile|Info")
	TArray<ETileState> TileStates;
	
public:
	/**
	 * Constructor de la clase que inicializa los parametros del actor
	 */
	AActorTile();

	//----------------------------------------------------------------------------------------------------------------//
	
	/**
	 * Getter del atributo TileType
	 * 
	 * @return Tipo de casilla
	 */
	const ETileType& GetType() const;

	/**
	 * Getter del atributo MovementCost
	 * 
	 * @return Coste de paso por la casilla
	 */
	int32 GetMovementCost() const;

	/**
	 * Getter de los atributos Unit/Settlement
	 * 
	 * @return Elemento situado en la casilla actual
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Tile|Elements")
	const AActorDamageableElement* GetElement() const;

	/**
	 * Getter del atributo TileStatess
	 * 
	 * @return Estado de la casilla
	 */
	const TArray<ETileState>& GetState() const;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Setter del atributo TileType
	 * 
	 * @param Type Tipo de casilla
	 */
	void SetType(const ETileType Type);

	/**
	 * Setter del atributo MovementCost
	 * 
	 * @param Cost Coste de paso por la casilla
	 */
	void SetMovementCost(const int32 Cost);

	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param States Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void SetState(const TArray<ETileState>& States);
	/**
	 * Setter del atributo TileStatess
	 * 
	 * @param State Estado de la casilla
	 */
	void SetState(const ETileState State);

	/**
	 * Anade los estados dados a los actuales
	 * 
	 * @param States Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void AddState(const TArray<ETileState>& States);
	/**
	 * Anade el estado dado a los actuales
	 * 
	 * @param State Estado de la casilla
	 */
	void AddState(const ETileState State);

	/**
	 * Elimina los estados dados de los actuales
	 * 
	 * @param States Estados de la casilla
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveState(const TArray<ETileState>& States);
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

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que transforma el tipo de casilla en un valor numerico
	 * 
	 * @param TileType Tipo de casilla
	 * @return Valor entero dado el tipo de casilla
	 */
	static int32 TileTypeToInt(const ETileType TileType);
	
	/**
	 * Metodo que transforma un valor numerico en el tipo de casilla
	 * 
	 * @param TileTypeVal Valor numerico del tipo de casilla
	 * @return Tipo de casilla
	 */
	static ETileType IntToTileType(const int32 TileTypeVal);
};

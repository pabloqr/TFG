// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "FResourceInfo.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "PawnFaction.generated.h"

class AActorSettlement;
enum class ESettlementState : uint8;
class AActorDamageableElement;

USTRUCT(BlueprintType)
struct FResourceCollection
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	FResource GatheredResource;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="ResourceCollection")
	TArray<FIntPoint> Tiles;

	FResourceCollection(): FResourceCollection(FResource(), TArray<FIntPoint>())
	{
	}

	FResourceCollection(const FResource& GatheredResource, const TArray<FIntPoint>& Tiles)
		: GatheredResource(GatheredResource),
		  Tiles(Tiles)
	{
	}
};

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
	TArray<int32> IdleSettlements;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<AActorUnit*> Units;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> ManualUnits;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TArray<int32> AutomaticUnits;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> MonetaryResources;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Elements")
	TMap<EResource, FResourceCollection> StrategicResources;

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
	int32 GetIndex() const { return Index; }
	float GetMoney() const { return Money; }
	float GetMoneyBalance() const { return MoneyBalance; }
	const TArray<AActorSettlement*>& GetSettlements() const { return Settlements; }
	int32 GetNumSettlements() const { return Settlements.Num(); }
	const TArray<int32>& GetIdleSettlements() const { return IdleSettlements; }
	const TMap<EResource, FResourceCollection>& GetMonetaryResources() const { return MonetaryResources; }
	const TMap<EResource, FResourceCollection>& GetStrategicResources() const { return StrategicResources; }
	const TArray<AActorUnit*>& GetUnits() const { return Units; }
	const TArray<int32>& GetManualUnits() const { return ManualUnits; }
	const TArray<int32>& GetAutomaticUnits() const { return AutomaticUnits; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetIndex(const int32 I) { Index = I; }

	//----------------------------------------------------------------------------------------------------------------//

	bool HasElement(const AActorDamageableElement* Element) const;

	UFUNCTION(BlueprintCallable)
	bool CanProduceUnit(const UDataTable* DataTable, const EUnitType UnitType) const;

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
	void OwnResource(const EResource Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void DisownResource(const EResource Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void AddResource(const FResource& Resource, const FIntPoint& Pos);

	UFUNCTION(BlueprintCallable)
	void RemoveResource(const FResource& Resource, const FIntPoint& Pos);

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAttackStats.h"
#include "FDamageableInfo.h"
#include "ActorDamageableElement.generated.h"

//--------------------------------------------------------------------------------------------------------------------//

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTriggered);

//--------------------------------------------------------------------------------------------------------------------//

UCLASS(Abstract)
class TFG_API AActorDamageableElement : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="DamageableElement")
	FDamageableInfo DamageableInfo;

public:
	/**
	 * Constructor por defecto
	 */
	AActorDamageableElement();

private:
	/**
	 * Metodo privado que actualiza los puntos de ataque y defensa del elemento
	 */
	void UpdateAttackAndDefenseParameters();

public:
	float GetBaseHealthPoints() const { return DamageableInfo.BaseHealthPoints; }
	float GetHealthPoints() const { return DamageableInfo.HealthPoints; }
	float GetStrengthPoints() const { return DamageableInfo.Stats.GetStrengthPoints(); }
	float GetAttackPoints() const { return DamageableInfo.Stats.AttackPoints; }
	float GetDefensePoints() const { return DamageableInfo.Stats.DefensePoints; }

	//----------------------------------------------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable)
	void SetFactionOwner(const int32 OwnerFaction) { DamageableInfo.Owner = OwnerFaction; }

	void SetHealthPoints(const float Points) { DamageableInfo.HealthPoints = Points; }
	void SetAttackPoints(const float Points) { DamageableInfo.Stats.AttackPoints = Points; }
	void SetDefensePoints(const float Points) { DamageableInfo.Stats.DefensePoints = Points; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que verifica si el elemento es propiedad del jugador actual
	 * 
	 * @return Si el elemtnos es propiedad del jugador actual
	 */
	UFUNCTION(BlueprintCallable)
	bool IsMine() const;

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que calcula el dano que se debe aplicar a cada elemento al realizar un ataque
	 * 
	 * @param IsAttacking
	 * @param Stats Elemento que se ataca
	 * @return Valores de dano a aplicar a cada uno de los elementos tras el ataque
	 */
	UFUNCTION(BlueprintCallable)
	float CalculateAttack(const bool IsAttacking, const FAttackStats& Stats) const;

	/**
	 * Metodo que hace efectivo un ataque sobre otro elemento
	 * 
	 * @param IsAttacking
	 * @param Stats Elemento que se ataca
	 */
	UFUNCTION(BlueprintCallable)
	void PerformAttack(const bool IsAttacking, const FAttackStats& Stats);

	/**
	 * Metodo que aplica un dano a un elemento. No se verifica si el elemento es destruido o no
	 * 
	 * @param Damage Dano a aplicar sobre un elemento
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ApplyDamage(const float Damage);

	//----------------------------------------------------------------------------------------------------------------//

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAttackTriggered OnAttackTriggered;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPlaceableElement.h"
#include "ActorDamageableElement.generated.h"

UCLASS(Abstract)
class TFG_API AActorDamageableElement : public AActorPlaceableElement
{
	GENERATED_BODY()
	
protected:
	/**
	 * Puntos de vida base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseHealthPoints;
	/**
	 * Puntos de ataque base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseAttackPoints;
	/**
	 * Puntos de defensa base del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float BaseDefensePoints;
	
	/**
	 * Puntos de vida del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float HealthPoints;
	/**
	 * Puntos de ataque del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float AttackPoints;
	/**
	 * Puntos de defensa del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float DefensePoints;
	
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

protected:
	/**
	 * Metodo ejecutado cuando el juego es iniciado o el actor es generado
	 */
	virtual void BeginPlay() override;

public:
	float GetHealthPoints() const { return HealthPoints; }
	float GetAttackPoints() const { return AttackPoints; }
	float GetDefensePoints() const { return DefensePoints; }

	//----------------------------------------------------------------------------------------------------------------//

	void SetHealthPoints(const float Points) { this->HealthPoints = Points; }
	void SetAttackPoints(const float Points) { this->AttackPoints = Points; }
	void SetDefensePoints(const float Points) { this->DefensePoints = Points; }

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo que calcula el dano que se debe aplicar a cada elemento al realizar un ataque
	 * 
	 * @param DamageableElement Elemento que se ataca
	 * @return Valores de dano a aplicar a cada uno de los elementos tras el ataque
	 */
	UFUNCTION(BlueprintCallable, Category="GameElement|Damage")
	FVector2D CalculateAttack(AActorDamageableElement* DamageableElement) const;
	
	/**
	 * Metodo que hace efectivo un ataque sobre otro elemento
	 * 
	 * @param DamageableElement Elemento que se ataca
	 */
	UFUNCTION(BlueprintCallable, Category="GameElement|Damage")
	void PerformAttack(AActorDamageableElement* DamageableElement);
	
	/**
	 * Metodo que aplica un dano a un elemento. No se verifica si el elemento es destruido o no
	 * 
	 * @param Damage Dano a aplicar sobre un elemento
	 */
	UFUNCTION(BlueprintCallable, Category="GameElement|Damage")
	void ApplyDamage(const float Damage);

	//----------------------------------------------------------------------------------------------------------------//

	/**
	 * Metodo ejecutado en cada frame
	 * 
	 * @param DeltaTime Tiempo transcurrido desde el ultimo frame
	 */
	virtual void Tick(float DeltaTime) override;
};

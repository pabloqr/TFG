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
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float HealthPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float AttackPoints;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Damage")
	float DefensePoints;
	
public:
	// Sets default values for this actor's properties
	AActorDamageableElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	float GetHealthPoints() const { return HealthPoints; }
	float GetAttackPoints() const { return AttackPoints; }
	float GetDefensePoints() const { return DefensePoints; }

	void SetHealthPoints(const float Points) { this->HealthPoints = Points; }
	void SetAttackPoints(const float Points) { this->AttackPoints = Points; }
	void SetDefensePoints(const float Points) { this->DefensePoints = Points; }

	void PerformAttack(AActorDamageableElement DamageableElement) const;
	void ApplyDamage(const float Damage);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorDamageableElement.h"
#include "ActorSettlement.generated.h"

UCLASS()
class TFG_API AActorSettlement : public AActorDamageableElement
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorSettlement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPlaceableElement.h"
#include "ActorDamageableElement.generated.h"

UCLASS()
class TFG_API AActorDamageableElement : public AActorPlaceableElement
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorDamageableElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

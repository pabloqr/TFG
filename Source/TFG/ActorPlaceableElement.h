// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FNamedElement.h"
#include "GameFramework/Actor.h"
#include "ActorPlaceableElement.generated.h"

UCLASS()
class TFG_API AActorPlaceableElement : public AActor, public FNamedElement
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorPlaceableElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

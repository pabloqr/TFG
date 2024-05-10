// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnFaction.h"
#include "GameFramework/Pawn.h"
#include "PMain.generated.h"

UCLASS()
class TFG_API APMain : public APawnFaction
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CMainAI.generated.h"

UCLASS()
class TFG_API ACMainAI : public AAIController
{
	GENERATED_BODY()

public:
	/**
	 * Constructor por defecto que inicializa los atributos de la clase
	 */
	ACMainAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void TurnStarted();

	UFUNCTION(BlueprintCallable)
	void TurnFinished();

	//----------------------------------------------------------------------------------------------------------------//

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

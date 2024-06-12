// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerMain.generated.h"

UENUM(BlueprintType)
enum class ESelectionState: uint8
{
	None = 0 UMETA(DisplayName="None"),
	NoSelection = 1 UMETA(DisplayName="NoSelection"),
	TileSelected = 2 UMETA(DisplayName="TileSelected"),
	UnitSelected = 3 UMETA(DisplayName="UnitSelected"),
	SettlementSelected = 4 UMETA(DisplayName="SettlementSelected"),
};

/**
 * 
 */
UCLASS()
class TFG_API ASPlayerMain : public APlayerState
{
	GENERATED_BODY()

protected:
	/**
	 * Modo de seleccion del jugador
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="PlayerState")
	ESelectionState SelectionState;

public:
	/**
	 * Constructor por defecto de la clase que inicializa los parametros de la instancia
	 */
	ASPlayerMain();
};

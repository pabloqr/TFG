// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUnit.h"
#include "GameFramework/Actor.h"
#include "ActorMilitaryUnit.generated.h"

UENUM(BlueprintType)
enum class EUnitType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Infantry = 1 UMETA(DisplayName="Infantry unit"),
	Armoured = 2 UMETA(DisplayName="Armoured unit"),
	AntiTank = 3 UMETA(DisplayName="Anti-tank unit")
};

UCLASS()
class TFG_API AActorMilitaryUnit : public AActorUnit
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Military")
	EUnitType Type;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Unit|Military")
	FElementIdentifier ProductionResource;

public:
	// Sets default values for this actor's properties
	AActorMilitaryUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

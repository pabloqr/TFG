// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPlaceableElement.h"
#include "ActorResource.generated.h"

UENUM()
enum class EResourceType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Strategic = 1 UMETA(DisplayName="Strategic"),
	Monetary = 2 UMETA(DisplayName="Monetary")
};

UCLASS()
class TFG_API AActorResource : public AActorPlaceableElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	EResourceType Type;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Resource")
	int32 Quantity;

public:
	// Sets default values for this actor's properties
	AActorResource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	EResourceType GetType() const { return Type; }
	int32 GetQuantity() const { return Quantity; }

	//----------------------------------------------------------------------------------------------------------------//

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

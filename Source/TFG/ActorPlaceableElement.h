// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorPlaceableElement.generated.h"

USTRUCT(BlueprintType)
struct FElementIdentifier
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Identifier")
	int32 Id;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Identifier")
	FString Name;
};

UCLASS()
class TFG_API AActorPlaceableElement : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Identifier")
	FElementIdentifier ElementIdentifier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Position")
	FIntPoint Pos2D;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="GameElement|Position")
	FVector2D MapPos2D;
	
public:
	// Sets default values for this actor's properties
	AActorPlaceableElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	const FElementIdentifier& GetIdentifier() const { return ElementIdentifier; }
	const FIntPoint& GetPos() const { return Pos2D; }
	const FVector2D& GetMapPos() const { return MapPos2D; }

	void SetIdentifier(const FElementIdentifier& Identifier) { this->ElementIdentifier = Identifier; }
	void SetPos(const FIntPoint& Pos, const FVector2D& MapPos);
	void SetPos(const FIntPoint& Pos) { this->Pos2D = Pos; }
	void SetMapPos(const FVector2D& Pos) { this->MapPos2D = Pos; }
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

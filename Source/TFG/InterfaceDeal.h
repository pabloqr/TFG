// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDealInfo.h"
#include "UObject/Interface.h"
#include "InterfaceDeal.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInterfaceDeal : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TFG_API IInterfaceDeal
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ProposeDeal(const FDealInfo& Deal) const = 0;
};

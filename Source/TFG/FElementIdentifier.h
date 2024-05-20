// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FElementIdentifier.generated.h"

USTRUCT(BlueprintType)
struct FElementIdentifier
{
	GENERATED_BODY()

	/**
	 * Identificador numerico del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Identifier")
	int32 Id;
	/**
	 * Identificador de texto del elemento
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Identifier")
	FString Name;

	/**
	 * Operador <
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor que el que se compara
	 */
	bool operator<(const FElementIdentifier& Other) const
	{
		return Id < Other.Id;
	}

	/**
	 * Operador >
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor que el que se compara
	 */
	bool operator>(const FElementIdentifier& Other) const
	{
		return !(*this < Other || *this == Other);
	}

	/**
	 * Operador ==
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es igual que el que se compara
	 */
	bool operator==(const FElementIdentifier& Other) const
	{
		return Id == Other.Id;
	}

	/**
	 * Operador !=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es diferente que el que se compara
	 */
	bool operator!=(const FElementIdentifier& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * Operador <=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es menor o igual que el que se compara
	 */
	bool operator<=(const FElementIdentifier& Other) const
	{
		return *this < Other || *this == Other;
	}

	/**
	 * Operador >=
	 * 
	 * @param Other Elemento de la clase
	 * @return El elemento actual es mayor o igual que el que se compara
	 */
	bool operator>=(const FElementIdentifier& Other) const
	{
		return *this > Other || *this == Other;
	}
};

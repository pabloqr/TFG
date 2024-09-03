// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * Clase para definir colas con prioridad
 */
template <class T>
class TPriorityQueue
{
	/**
	 * Lista de elementos parametrizada, el tipo de dato debe proporcionar los operadores implementados
	 */
	TArray<T> Elements;

public:
	/**
	 * Constructor por defecto de la clase
	 */
	TPriorityQueue(): Elements(TArray<T>())
	{
	}

	/**
	 * Metodo que verifica si un elemento esta contenido en la cola
	 * 
	 * @param Element Elemento a verificar
	 * @return Si el elemento esta contenido en la cola
	 */
	bool Contains(const T& Element) const
	{
		return Elements.Contains(Element);
	}

	/**
	 * Metodo que devuelve si la cadena esta vacia o no
	 * 
	 * @return La cadena esta vacia
	 */
	bool IsEmpty() const
	{
		return Elements.Num() == 0;
	}

	/**
	 * Metodo que inserta de forma ordenada el elemento dado segun su prioridad
	 * 
	 * @param Element Elemento a insertar
	 */
	void Push(const T& Element)
	{
		// Se comprueba si el elemento tiene menos prioridad que el ultimo para evitar recorrer todo el array
		if (IsEmpty() || Element >= Elements.Last()) Elements.Add(Element);
		else
			for (int32 i = 0; i < Elements.Num(); ++i)
			{
				// Si el elemento tiene menos prioridad se inserta y se finaliza
				if (Elements[i] >= Element)
				{
					Elements.Insert(Element, i);
					break;
				}
			}
	}

	/**
	 * Metodo que devuelve el elemento con mayor prioridad del array
	 * 
	 * @return El primer elemento del array
	 */
	T Pop()
	{
		T Element = !IsEmpty() ? Elements[0] : T();
		Elements.RemoveAt(0);

		return Element;
	}

	/**
	 * Metodo que elimina todas las entradas de la lista
	 */
	void Empty()
	{
		Elements.Empty();
	}
};

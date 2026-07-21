// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implement on any actor the player should be able to interact with.
 */
class IInteractable
{
	GENERATED_BODY()

public:

	/** Called on the target actor when a player interacts with it (e.g. presses the Interact key while looking at it). */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
};

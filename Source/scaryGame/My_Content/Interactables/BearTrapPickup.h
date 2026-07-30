// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "BearTrapPickup.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that adds bear traps to the interacting AHorrorCharacter's carried count, then
 * destroys itself. Subclass as a Blueprint to assign a mesh and Amount.
 */
UCLASS()
class SCARYGAME_API ABearTrapPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABearTrapPickup();

	/** Number of bear traps this pickup grants */
	UPROPERTY(EditAnywhere, Category = "Trap")
	int32 Amount = 1;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

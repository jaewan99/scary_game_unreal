// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "NoiseDecoyPickup.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that adds noise decoys to the interacting AHorrorCharacter's carried count, then
 * destroys itself. Subclass as a Blueprint to assign a mesh and Amount.
 */
UCLASS()
class SCARYGAME_API ANoiseDecoyPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ANoiseDecoyPickup();

	/** Number of noise decoys this pickup grants */
	UPROPERTY(EditAnywhere, Category = "Noise Decoy")
	int32 Amount = 1;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "BatteryPickup.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that recharges the interacting AHorrorCharacter's flashlight battery, then destroys itself.
 * Subclass as a Blueprint to assign a mesh and ChargeAmount.
 */
UCLASS()
class SCARYGAME_API ABatteryPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABatteryPickup();

	/** Seconds of flashlight charge this pickup restores */
	UPROPERTY(EditAnywhere, Category = "Battery")
	float ChargeAmount = 30.0f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

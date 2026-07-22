// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "BandagePickup.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that heals the interacting AHorrorCharacter's grab-injury speed penalty, then
 * destroys itself. Subclass as a Blueprint to assign a mesh.
 */
UCLASS()
class SCARYGAME_API ABandagePickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABandagePickup();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

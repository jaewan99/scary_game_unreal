// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "BarricadeMaterialPickup.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that adds barricade material (planks, furniture, etc.) to the interacting
 * AHorrorCharacter's carried count, then destroys itself. Subclass as a Blueprint to assign a
 * mesh and Amount.
 */
UCLASS()
class SCARYGAME_API ABarricadeMaterialPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABarricadeMaterialPickup();

	/** Amount of barricade material this pickup grants */
	UPROPERTY(EditAnywhere, Category = "Barricade")
	int32 Amount = 1;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

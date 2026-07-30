// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "KeyItem.generated.h"

class UStaticMeshComponent;

/**
 * A pickup that adds KeyID to the interacting AHorrorCharacter's collected keys, then destroys itself.
 * Subclass as a Blueprint to assign a mesh and KeyID.
 */
UCLASS()
class SCARYGAME_API AKeyItem : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AKeyItem();

	/** Identifier checked against doors that require this key */
	UPROPERTY(EditAnywhere, Category = "Key")
	FName KeyID;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

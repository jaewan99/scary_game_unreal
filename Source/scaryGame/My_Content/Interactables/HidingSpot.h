// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "HidingSpot.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class AHorrorCharacter;

/**
 * A wardrobe/locker/etc. the player can hide inside. Interacting while outside moves the
 * player to HidePoint (place it enclosed by Mesh's geometry so enemy sight traces are blocked)
 * and disables their movement; interacting again restores them to where they entered from.
 * Subclass as a Blueprint to assign a mesh and position HidePoint.
 */
UCLASS()
class SCARYGAME_API AHidingSpot : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AHidingSpot();

	/** True while a player is currently hidden here */
	UPROPERTY(BlueprintReadOnly, Category = "Hiding")
	bool bOccupied = false;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	/** Where the player is moved to while hidden; position this inside Mesh's bounds in the Blueprint/level editor */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* HidePoint;

	/** The character currently hiding here, if any */
	TWeakObjectPtr<AHorrorCharacter> HiddenCharacter;

	/** Where HiddenCharacter was standing before they hid, restored when they exit */
	FTransform SavedCharacterTransform;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Passes control to Blueprint when a player enters hiding here (e.g. to play a door-close animation) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void BP_OnPlayerEnteredHiding(AActor* Interactor);

	/** Passes control to Blueprint when a player exits hiding here */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void BP_OnPlayerExitedHiding(AActor* Interactor);
};

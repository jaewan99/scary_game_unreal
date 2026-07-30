// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "InteractableDoor.generated.h"

class UStaticMeshComponent;

/**
 * A door that stays locked until interacted with by a AHorrorCharacter carrying RequiredKeyID.
 * Once unlocked (or if never locked), each interact toggles it open/closed.
 * Subclass as a Blueprint to assign a mesh and implement the open/close visuals.
 */
UCLASS()
class SCARYGAME_API AInteractableDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableDoor();

	/** If true, this door requires RequiredKeyID before it can be opened */
	UPROPERTY(EditAnywhere, Category = "Door")
	bool bLocked = true;

	/** Key identifier required to unlock this door */
	UPROPERTY(EditAnywhere, Category = "Door", meta = (EditCondition = "bLocked"))
	FName RequiredKeyID;

	/** Unlocks the door without requiring RequiredKeyID; call from a puzzle lock's BP_OnUnlocked (e.g. a paired ACombinationLock/ASequenceLock/ALockpickLock) */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Unlock();

	/** True while this door is barricaded; blocks Interact from toggling it open/closed until the barricade is removed or broken */
	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bBarricaded = false;

	/** How many times the barricade can be damaged (e.g. by an enemy) before it breaks; 0 = never breaks on its own */
	UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "0"))
	int32 BarricadeStrength = 3;

	/** Barricades the closed door shut; returns false if it's already barricaded or currently open */
	UFUNCTION(BlueprintCallable, Category = "Door")
	bool BarricadeDoor(AActor* Interactor);

	/** Removes the barricade, if any, allowing the door to be opened/closed again */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void RemoveBarricade();

	/** Damages the barricade (e.g. from an enemy battering it); returns true if the barricade broke as a result */
	UFUNCTION(BlueprintCallable, Category = "Door")
	bool DamageBarricade();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	/** If true, the door is currently open */
	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bOpen = false;

	/** Barricade hits absorbed so far */
	int32 BarricadeDamageTaken = 0;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Passes control to Blueprint to play the door's open/close visuals */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnToggleOpen(bool bNewOpen);

	/** Passes control to Blueprint to react to an interact attempt while still locked */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnInteractWhileLocked();

	/** Passes control to Blueprint to react to an interact attempt while barricaded */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnInteractWhileBarricaded();

	/** Passes control to Blueprint to play the barricading visuals */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnBarricaded(AActor* Interactor);

	/** Passes control to Blueprint when the barricade is removed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnBarricadeRemoved();

	/** Passes control to Blueprint when the barricade breaks from damage */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnBarricadeBroken();
};

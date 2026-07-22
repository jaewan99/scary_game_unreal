// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "SequenceLock.generated.h"

class UStaticMeshComponent;

/**
 * A lock solved by entering symbols in a specific order (e.g. runes carved into a door, or
 * emblems that must be pressed in sequence). Call InputSymbol as each symbol is chosen (from
 * UI buttons or clickable meshes); an incorrect symbol resets the attempt, and matching every
 * entry in CorrectSequence in order unlocks it.
 */
UCLASS()
class SCARYGAME_API ASequenceLock : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ASequenceLock();

	/** The symbol indices (into a designer-defined symbol set) that must be entered in order */
	UPROPERTY(EditAnywhere, Category = "Lock")
	TArray<int32> CorrectSequence;

	/** True once every entry in CorrectSequence has been entered in order */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	bool bUnlocked = false;

	/** Symbols entered correctly so far this attempt */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	TArray<int32> EnteredSequence;

	/** Enters the next symbol; resets the attempt on a wrong entry, unlocks on a completed correct sequence */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void InputSymbol(int32 SymbolIndex);

	/** Clears any symbols entered so far without affecting the unlocked state */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void ResetSequence();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Passes control to Blueprint when the player interacts with the lock (e.g. to show the symbol UI) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnInteractStart(AActor* Interactor);

	/** Passes control to Blueprint whenever a correct-so-far symbol is entered, so a UI widget can highlight it */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnSymbolEntered(int32 SymbolIndex, int32 EntryIndex);

	/** Passes control to Blueprint when a wrong symbol resets the attempt */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnSequenceFailed();

	/** Passes control to Blueprint when the sequence is solved */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnUnlocked();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "LockpickLock.generated.h"

class UStaticMeshComponent;

/**
 * A lock solved by a timing-based lockpicking minigame: StartPicking randomizes a "sweet spot"
 * along a 0-1 range, and AttemptPick checks a UI-driven timing value (e.g. where a moving needle
 * was when the player released input) against it. Running out of attempts breaks the pick.
 */
UCLASS()
class SCARYGAME_API ALockpickLock : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ALockpickLock();

	/** Fraction (0-0.5) of the pick range counted as a success either side of SweetSpotCenter */
	UPROPERTY(EditAnywhere, Category = "Lock", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float SweetSpotWidth = 0.1f;

	/** How many failed attempts before the pick breaks and the lock can no longer be picked; 0 = unlimited */
	UPROPERTY(EditAnywhere, Category = "Lock", meta = (ClampMin = "0"))
	int32 MaxAttempts = 3;

	/** True once AttemptPick has succeeded */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	bool bUnlocked = false;

	/** True once MaxAttempts failed attempts have been used up */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	bool bPickBroken = false;

	/** Failed attempts used so far this pick */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	int32 AttemptsUsed = 0;

	/** Center of the current sweet spot (0-1); randomized by StartPicking */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	float SweetSpotCenter = 0.5f;

	/** Randomizes SweetSpotCenter and resets AttemptsUsed; call when the picking UI opens */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void StartPicking();

	/** Checks TimingValue (0-1) against the sweet spot; unlocks on a hit, counts a failure on a miss */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	bool AttemptPick(float TimingValue);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Passes control to Blueprint when the player interacts with the lock (e.g. to show the picking UI) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnInteractStart(AActor* Interactor);

	/** Passes control to Blueprint on a failed pick attempt */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnPickFailed(int32 AttemptsRemaining);

	/** Passes control to Blueprint once the pick breaks (MaxAttempts used up) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnPickBroken();

	/** Passes control to Blueprint when the lock is picked successfully */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnUnlocked();
};

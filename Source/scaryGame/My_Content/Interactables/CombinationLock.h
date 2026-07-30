// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "CombinationLock.generated.h"

class UStaticMeshComponent;

/**
 * A numeric combination lock of configurable length. Each digit is cycled independently
 * via IncrementDigit/DecrementDigit (e.g. bound to up/down UI buttons); once every digit
 * matches CorrectCombination, the lock unlocks.
 */
UCLASS()
class SCARYGAME_API ACombinationLock : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ACombinationLock();

	/** Number of digits in the combination (e.g. 3 for a 3-digit lock) */
	UPROPERTY(EditAnywhere, Category = "Lock", meta = (ClampMin = "1"))
	int32 NumDigits = 3;

	/** Each digit wraps between 0 and DigitRange - 1 (e.g. 10 for digits 0-9) */
	UPROPERTY(EditAnywhere, Category = "Lock", meta = (ClampMin = "2"))
	int32 DigitRange = 10;

	/** The combination that unlocks this lock; padded/trimmed to NumDigits entries on BeginPlay */
	UPROPERTY(EditAnywhere, Category = "Lock")
	TArray<int32> CorrectCombination;

	/** True once the dialed-in digits match CorrectCombination */
	UPROPERTY(BlueprintReadOnly, Category = "Lock")
	bool bUnlocked = false;

	/** Increments the digit at DigitIndex, wrapping around at DigitRange */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void IncrementDigit(int32 DigitIndex);

	/** Decrements the digit at DigitIndex, wrapping around at DigitRange */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void DecrementDigit(int32 DigitIndex);

	/** Returns the currently dialed-in value of the digit at DigitIndex */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lock")
	int32 GetDigitValue(int32 DigitIndex) const;

	/** Resets every digit back to zero */
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void ResetCombination();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	/** The digits the player has currently dialed in */
	UPROPERTY()
	TArray<int32> CurrentDigits;

	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Checks CurrentDigits against CorrectCombination and unlocks if they match */
	void CheckCombination();

	/** Passes control to Blueprint when the player interacts with the lock (e.g. to zoom in and show the combination UI) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnInteractStart(AActor* Interactor);

	/** Passes control to Blueprint whenever a digit changes, so a UI widget can refresh it */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnDigitChanged(int32 DigitIndex, int32 NewValue);

	/** Passes control to Blueprint when the combination is solved */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void BP_OnUnlocked();
};

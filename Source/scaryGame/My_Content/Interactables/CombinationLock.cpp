// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/CombinationLock.h"
#include "Components/StaticMeshComponent.h"

ACombinationLock::ACombinationLock()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ACombinationLock::BeginPlay()
{
	Super::BeginPlay();

	CurrentDigits.SetNumZeroed(NumDigits);

	// pad or trim the designer-set combination so it always matches NumDigits
	CorrectCombination.SetNumZeroed(NumDigits);
}

void ACombinationLock::Interact_Implementation(AActor* Interactor)
{
	if (!bUnlocked)
	{
		BP_OnInteractStart(Interactor);
	}
}

void ACombinationLock::IncrementDigit(int32 DigitIndex)
{
	if (bUnlocked || !CurrentDigits.IsValidIndex(DigitIndex))
	{
		return;
	}

	CurrentDigits[DigitIndex] = (CurrentDigits[DigitIndex] + 1) % DigitRange;
	BP_OnDigitChanged(DigitIndex, CurrentDigits[DigitIndex]);
	CheckCombination();
}

void ACombinationLock::DecrementDigit(int32 DigitIndex)
{
	if (bUnlocked || !CurrentDigits.IsValidIndex(DigitIndex))
	{
		return;
	}

	CurrentDigits[DigitIndex] = (CurrentDigits[DigitIndex] - 1 + DigitRange) % DigitRange;
	BP_OnDigitChanged(DigitIndex, CurrentDigits[DigitIndex]);
	CheckCombination();
}

int32 ACombinationLock::GetDigitValue(int32 DigitIndex) const
{
	return CurrentDigits.IsValidIndex(DigitIndex) ? CurrentDigits[DigitIndex] : 0;
}

void ACombinationLock::ResetCombination()
{
	for (int32 Index = 0; Index < CurrentDigits.Num(); ++Index)
	{
		CurrentDigits[Index] = 0;
		BP_OnDigitChanged(Index, 0);
	}
}

void ACombinationLock::CheckCombination()
{
	if (CurrentDigits == CorrectCombination)
	{
		bUnlocked = true;
		BP_OnUnlocked();
	}
}

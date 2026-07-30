// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/LockpickLock.h"
#include "Components/StaticMeshComponent.h"

ALockpickLock::ALockpickLock()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ALockpickLock::Interact_Implementation(AActor* Interactor)
{
	if (!bUnlocked && !bPickBroken)
	{
		BP_OnInteractStart(Interactor);
	}
}

void ALockpickLock::StartPicking()
{
	if (bUnlocked || bPickBroken)
	{
		return;
	}

	AttemptsUsed = 0;
	SweetSpotCenter = FMath::FRandRange(SweetSpotWidth, 1.0f - SweetSpotWidth);
}

bool ALockpickLock::AttemptPick(float TimingValue)
{
	if (bUnlocked || bPickBroken)
	{
		return false;
	}

	if (FMath::Abs(TimingValue - SweetSpotCenter) <= SweetSpotWidth)
	{
		bUnlocked = true;
		BP_OnUnlocked();
		return true;
	}

	++AttemptsUsed;

	if (MaxAttempts > 0 && AttemptsUsed >= MaxAttempts)
	{
		bPickBroken = true;
		BP_OnPickBroken();
	}
	else
	{
		BP_OnPickFailed(MaxAttempts > 0 ? MaxAttempts - AttemptsUsed : -1);
	}

	return false;
}

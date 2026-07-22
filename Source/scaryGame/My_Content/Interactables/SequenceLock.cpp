// Fill out your copyright notice in the Description page of Project Settings.

#include "SequenceLock.h"
#include "Components/StaticMeshComponent.h"

ASequenceLock::ASequenceLock()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ASequenceLock::Interact_Implementation(AActor* Interactor)
{
	if (!bUnlocked)
	{
		BP_OnInteractStart(Interactor);
	}
}

void ASequenceLock::InputSymbol(int32 SymbolIndex)
{
	if (bUnlocked || !CorrectSequence.IsValidIndex(EnteredSequence.Num()))
	{
		return;
	}

	if (SymbolIndex != CorrectSequence[EnteredSequence.Num()])
	{
		ResetSequence();
		BP_OnSequenceFailed();
		return;
	}

	EnteredSequence.Add(SymbolIndex);
	BP_OnSymbolEntered(SymbolIndex, EnteredSequence.Num() - 1);

	if (EnteredSequence.Num() == CorrectSequence.Num())
	{
		bUnlocked = true;
		BP_OnUnlocked();
	}
}

void ASequenceLock::ResetSequence()
{
	EnteredSequence.Reset();
}

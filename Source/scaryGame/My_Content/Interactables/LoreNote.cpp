// Fill out your copyright notice in the Description page of Project Settings.

#include "LoreNote.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ALoreNote::ALoreNote()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ALoreNote::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->CollectLoreNote(NoteID, NoteText);
		Destroy();
	}
}

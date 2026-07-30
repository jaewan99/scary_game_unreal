// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/HidingSpot.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

AHidingSpot::AHidingSpot()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	HidePoint = CreateDefaultSubobject<USceneComponent>(TEXT("HidePoint"));
	HidePoint->SetupAttachment(Mesh);
}

void AHidingSpot::Interact_Implementation(AActor* Interactor)
{
	AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor);
	if (!HorrorCharacter)
	{
		return;
	}

	if (bOccupied)
	{
		if (HiddenCharacter.Get() != HorrorCharacter)
		{
			return;
		}

		HorrorCharacter->ExitHiding(SavedCharacterTransform);
		HiddenCharacter = nullptr;
		bOccupied = false;
		BP_OnPlayerExitedHiding(HorrorCharacter);
	}
	else
	{
		SavedCharacterTransform = HorrorCharacter->GetActorTransform();
		HorrorCharacter->EnterHiding(HidePoint->GetComponentTransform());
		HiddenCharacter = HorrorCharacter;
		bOccupied = true;
		BP_OnPlayerEnteredHiding(HorrorCharacter);
	}
}

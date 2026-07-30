// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/InteractableDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

AInteractableDoor::AInteractableDoor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void AInteractableDoor::Interact_Implementation(AActor* Interactor)
{
	if (bBarricaded)
	{
		BP_OnInteractWhileBarricaded();
		return;
	}

	if (bLocked)
	{
		const AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor);
		if (HorrorCharacter && HorrorCharacter->HasKey(RequiredKeyID))
		{
			bLocked = false;
		}
		else
		{
			BP_OnInteractWhileLocked();
			return;
		}
	}

	bOpen = !bOpen;
	BP_OnToggleOpen(bOpen);
}

void AInteractableDoor::Unlock()
{
	bLocked = false;
}

bool AInteractableDoor::BarricadeDoor(AActor* Interactor)
{
	if (bBarricaded || bOpen)
	{
		return false;
	}

	bBarricaded = true;
	BarricadeDamageTaken = 0;
	BP_OnBarricaded(Interactor);
	return true;
}

void AInteractableDoor::RemoveBarricade()
{
	if (!bBarricaded)
	{
		return;
	}

	bBarricaded = false;
	BarricadeDamageTaken = 0;
	BP_OnBarricadeRemoved();
}

bool AInteractableDoor::DamageBarricade()
{
	if (!bBarricaded)
	{
		return false;
	}

	++BarricadeDamageTaken;

	if (BarricadeStrength > 0 && BarricadeDamageTaken >= BarricadeStrength)
	{
		bBarricaded = false;
		BarricadeDamageTaken = 0;
		BP_OnBarricadeBroken();
		return true;
	}

	return false;
}

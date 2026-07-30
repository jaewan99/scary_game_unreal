// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/NoiseDecoyPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ANoiseDecoyPickup::ANoiseDecoyPickup()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ANoiseDecoyPickup::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->AddNoiseDecoy(Amount);
		Destroy();
	}
}

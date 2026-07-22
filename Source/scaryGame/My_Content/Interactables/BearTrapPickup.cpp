// Fill out your copyright notice in the Description page of Project Settings.

#include "BearTrapPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ABearTrapPickup::ABearTrapPickup()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ABearTrapPickup::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->AddBearTrap(Amount);
		Destroy();
	}
}

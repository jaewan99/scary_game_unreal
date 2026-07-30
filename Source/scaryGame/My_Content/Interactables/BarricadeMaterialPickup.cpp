// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interactables/BarricadeMaterialPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ABarricadeMaterialPickup::ABarricadeMaterialPickup()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ABarricadeMaterialPickup::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->AddBarricadeMaterial(Amount);
		Destroy();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ABatteryPickup::ABatteryPickup()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ABatteryPickup::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->AddBatteryCharge(ChargeAmount);
		Destroy();
	}
}

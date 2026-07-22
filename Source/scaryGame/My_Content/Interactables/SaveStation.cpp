// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveStation.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

ASaveStation::ASaveStation()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ASaveStation::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->SaveProgress(SaveSlotName, UserIndex);
		BP_OnSaved();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "KeyItem.h"
#include "Components/StaticMeshComponent.h"
#include "Variant_Horror/HorrorCharacter.h"

AKeyItem::AKeyItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void AKeyItem::Interact_Implementation(AActor* Interactor)
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Interactor))
	{
		HorrorCharacter->AddKey(KeyID);
		Destroy();
	}
}

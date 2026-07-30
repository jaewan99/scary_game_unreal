// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Content/Interaction/IInteractable.h"
#include "SaveStation.generated.h"

class UStaticMeshComponent;

/**
 * An interactable (e.g. a mirror, phone, or typewriter) that saves the interacting
 * AHorrorCharacter's progress to SaveSlotName when interacted with.
 * Subclass as a Blueprint to assign a mesh.
 */
UCLASS()
class SCARYGAME_API ASaveStation : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ASaveStation();

	/** Save slot name used when a player interacts with this station */
	UPROPERTY(EditAnywhere, Category = "Save")
	FString SaveSlotName = TEXT("HorrorSaveSlot");

	/** Save user index passed to the save system */
	UPROPERTY(EditAnywhere, Category = "Save")
	int32 UserIndex = 0;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Passes control to Blueprint once the save completes (e.g. to play a save icon/sound) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Save")
	void BP_OnSaved();
};

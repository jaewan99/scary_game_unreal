// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "LoreNote.generated.h"

class UStaticMeshComponent;

/**
 * A readable pickup that adds NoteID/NoteText to the interacting AHorrorCharacter's collected
 * lore notes (e.g. for a journal UI), then destroys itself. Subclass as a Blueprint to assign a
 * mesh, NoteID, and NoteText.
 */
UCLASS()
class SCARYGAME_API ALoreNote : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ALoreNote();

	/** Unique identifier for this note, checked against the player's collected notes to avoid duplicates */
	UPROPERTY(EditAnywhere, Category = "Lore")
	FName NoteID;

	/** The note's readable text, passed along when collected so a journal UI can display it */
	UPROPERTY(EditAnywhere, Category = "Lore", meta = (MultiLine = "true"))
	FText NoteText;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

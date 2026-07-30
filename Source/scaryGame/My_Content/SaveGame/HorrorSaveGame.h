// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "My_Content/Inventory/InventoryTypes.h"
#include "HorrorSaveGame.generated.h"

/**
 * Stores an AHorrorCharacter's progress (collected items, battery, injury, transform) for a
 * save slot. Written/read by AHorrorCharacter::SaveProgress/LoadProgress.
 */
UCLASS()
class SCARYGAME_API UHorrorSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Everything held in the player's inventory component, saved generically as ID+count stacks (keys today, more later) */
	UPROPERTY(VisibleAnywhere, Category = "Save")
	TArray<FInventoryItemStack> InventoryItems;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	TArray<FName> CollectedNoteIDs;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	int32 NoiseDecoyCount = 0;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	int32 BarricadeMaterialCount = 0;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	int32 BearTrapCount = 0;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	float BatteryMeter = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	bool bInjured = false;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	FTransform PlayerTransform;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	FName LevelName;
};

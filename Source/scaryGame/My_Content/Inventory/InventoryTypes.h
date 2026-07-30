// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

/**
 * One stack of items in an inventory: an identifier plus how many are held.
 *
 * Deliberately tiny and self-contained so it can be shared by the runtime
 * inventory (UInventoryComponent) and the save game (UHorrorSaveGame) without
 * either one depending on the other. New item types are just new ItemIDs -
 * no new fields, no schema changes.
 */
USTRUCT(BlueprintType)
struct FInventoryItemStack
{
	GENERATED_BODY()

	/** Identifier for the item type (e.g. "Key_Cellar", "NoiseDecoy"). Matched against doors, recipes, UI, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemID = NAME_None;

	/** How many of this item are held. Always >= 1 while the stack exists. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Count = 1;

	FInventoryItemStack() = default;

	FInventoryItemStack(FName InItemID, int32 InCount)
		: ItemID(InItemID)
		, Count(InCount)
	{
	}
};

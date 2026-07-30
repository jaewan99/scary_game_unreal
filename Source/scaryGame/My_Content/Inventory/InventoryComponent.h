// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "My_Content/Inventory/InventoryTypes.h"
#include "InventoryComponent.generated.h"

/** Fired whenever the inventory contents change (item added, removed, or reloaded). UI binds to this to refresh. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedDelegate);

/**
 * Modular, data-driven item storage that can be attached to any actor (the player,
 * an NPC, a lootable chest, ...). It knows nothing about keys, decoys, or any
 * specific game system - it just stores (ItemID, Count) stacks and answers questions
 * about them. Game systems layer meaning on top by choosing ItemIDs.
 *
 * Because it's a UActorComponent it is composable (drop it on any actor via C++ or
 * Blueprint) and owns its own state, so adding, querying, and persisting items is
 * all handled in one place instead of being scattered across each owning actor.
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class SCARYGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/** Adds Count of ItemID, stacking onto an existing stack if present. Returns the new total held. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(FName ItemID, int32 Count = 1);

	/** Removes up to Count of ItemID; drops the stack when it hits zero. Returns how many were actually removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(FName ItemID, int32 Count = 1);

	/** Returns true if at least one of ItemID is held. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemID) const;

	/** Returns how many of ItemID are held (0 if none). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	/** Returns a copy of every stack held; convenient for driving UI lists from Blueprint. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	TArray<FInventoryItemStack> GetAllItems() const { return Items; }

	/** Read-only view of the stacks; used by the save system to snapshot contents without copying to Blueprint. */
	const TArray<FInventoryItemStack>& GetItems() const { return Items; }

	/** Replaces the entire inventory in one shot (used when loading a save). Broadcasts OnInventoryChanged once. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItems(const TArray<FInventoryItemStack>& NewItems);

	/** Broadcast after any change so listeners (UI, quest logic) can react. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryChangedDelegate OnInventoryChanged;

protected:
	/** The stacks currently held. This is the single source of truth for what the owner is carrying. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItemStack> Items;

	/** Returns the index of ItemID's stack in Items, or INDEX_NONE. */
	int32 IndexOfItem(FName ItemID) const;
};

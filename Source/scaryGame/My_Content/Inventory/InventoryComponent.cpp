// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Inventory/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	// Pure data store; no per-frame work needed.
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UInventoryComponent::IndexOfItem(FName ItemID) const
{
	return Items.IndexOfByPredicate([ItemID](const FInventoryItemStack& Stack)
	{
		return Stack.ItemID == ItemID;
	});
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Count)
{
	if (ItemID.IsNone() || Count <= 0)
	{
		return GetItemCount(ItemID);
	}

	const int32 Index = IndexOfItem(ItemID);
	if (Index == INDEX_NONE)
	{
		Items.Emplace(ItemID, Count);
	}
	else
	{
		Items[Index].Count += Count;
	}

	OnInventoryChanged.Broadcast();
	return GetItemCount(ItemID);
}

int32 UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
	if (ItemID.IsNone() || Count <= 0)
	{
		return 0;
	}

	const int32 Index = IndexOfItem(ItemID);
	if (Index == INDEX_NONE)
	{
		return 0;
	}

	const int32 Removed = FMath::Min(Count, Items[Index].Count);
	Items[Index].Count -= Removed;
	if (Items[Index].Count <= 0)
	{
		Items.RemoveAt(Index);
	}

	if (Removed > 0)
	{
		OnInventoryChanged.Broadcast();
	}
	return Removed;
}

bool UInventoryComponent::HasItem(FName ItemID) const
{
	return GetItemCount(ItemID) > 0;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
	const int32 Index = IndexOfItem(ItemID);
	return Index == INDEX_NONE ? 0 : Items[Index].Count;
}

void UInventoryComponent::SetItems(const TArray<FInventoryItemStack>& NewItems)
{
	Items = NewItems;
	OnInventoryChanged.Broadcast();
}

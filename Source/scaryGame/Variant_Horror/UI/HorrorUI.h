// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HorrorUI.generated.h"

class AHorrorCharacter;

/**
 *  Simple UI for a first person horror game
 *  Manages character sprint meter display
 */
UCLASS(abstract)
class SCARYGAME_API UHorrorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Sets up delegate listeners for the passed character */
	void SetupCharacter(AHorrorCharacter* HorrorCharacter);

	/** Called when the character's sprint meter is updated */
	UFUNCTION()
	void OnSprintMeterUpdated(float Percent);

	/** Called when the character's sprint state changes */
	UFUNCTION()
	void OnSprintStateChanged(bool bSprinting);

	/** Called when the character's flashlight battery meter is updated */
	UFUNCTION()
	void OnBatteryMeterUpdated(float Percent);

	/** Called when the character's flashlight is switched on or off */
	UFUNCTION()
	void OnFlashlightStateChanged(bool bOn);

	/** Called when the character's fear meter is updated */
	UFUNCTION()
	void OnFearMeterUpdated(float FearLevel);

	/** Called when the character's fear level crosses the high-fear threshold in either direction */
	UFUNCTION()
	void OnHighFearStateChanged(bool bHighFear);

	/** Called when the character collects a new lore note */
	UFUNCTION()
	void OnLoreNoteCollected(FName NoteID, FText NoteText);

	/** Called when the character's grab escape progress updates */
	UFUNCTION()
	void OnGrabMeterUpdated(float Percent);

	/** Called when the character is grabbed or the grab ends */
	UFUNCTION()
	void OnGrabStateChanged(bool bGrabbed);

	/** Called when the character fails to escape a grab in time */
	UFUNCTION()
	void OnGrabFailed();

protected:

	/** Passes control to Blueprint to update the sprint meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Sprint Meter Updated"))
	void BP_SprintMeterUpdated(float Percent);

	/** Passes control to Blueprint to update the sprint meter status */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Sprint State Changed"))
	void BP_SprintStateChanged(bool bSprinting);

	/** Passes control to Blueprint to update the battery meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Battery Meter Updated"))
	void BP_BatteryMeterUpdated(float Percent);

	/** Passes control to Blueprint to update the flashlight on/off status */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Flashlight State Changed"))
	void BP_FlashlightStateChanged(bool bOn);

	/** Passes control to Blueprint to update the fear meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Fear Meter Updated"))
	void BP_FearMeterUpdated(float FearLevel);

	/** Passes control to Blueprint to react to crossing the high-fear threshold */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "High Fear State Changed"))
	void BP_HighFearStateChanged(bool bHighFear);

	/** Passes control to Blueprint to show a collected lore note (e.g. a toast or journal update) */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Lore Note Collected"))
	void BP_LoreNoteCollected(FName NoteID, const FText& NoteText);

	/** Passes control to Blueprint to update the grab escape meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Grab Meter Updated"))
	void BP_GrabMeterUpdated(float Percent);

	/** Passes control to Blueprint to show/hide the grab escape prompt */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Grab State Changed"))
	void BP_GrabStateChanged(bool bGrabbed);

	/** Passes control to Blueprint when the player fails to escape a grab in time (e.g. to show a game-over screen) */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Grab Failed"))
	void BP_GrabFailed();
};

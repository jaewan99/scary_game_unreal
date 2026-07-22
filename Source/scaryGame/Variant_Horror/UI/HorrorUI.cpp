// Copyright Epic Games, Inc. All Rights Reserved.


#include "HorrorUI.h"
#include "HorrorCharacter.h"

void UHorrorUI::SetupCharacter(AHorrorCharacter* HorrorCharacter)
{
	HorrorCharacter->OnSprintMeterUpdated.AddDynamic(this, &UHorrorUI::OnSprintMeterUpdated);
	HorrorCharacter->OnSprintStateChanged.AddDynamic(this, &UHorrorUI::OnSprintStateChanged);
	HorrorCharacter->OnBatteryMeterUpdated.AddDynamic(this, &UHorrorUI::OnBatteryMeterUpdated);
	HorrorCharacter->OnFlashlightStateChanged.AddDynamic(this, &UHorrorUI::OnFlashlightStateChanged);
	HorrorCharacter->OnFearMeterUpdated.AddDynamic(this, &UHorrorUI::OnFearMeterUpdated);
	HorrorCharacter->OnHighFearStateChanged.AddDynamic(this, &UHorrorUI::OnHighFearStateChanged);
	HorrorCharacter->OnLoreNoteCollected.AddDynamic(this, &UHorrorUI::OnLoreNoteCollected);
	HorrorCharacter->OnGrabMeterUpdated.AddDynamic(this, &UHorrorUI::OnGrabMeterUpdated);
	HorrorCharacter->OnGrabStateChanged.AddDynamic(this, &UHorrorUI::OnGrabStateChanged);
	HorrorCharacter->OnGrabFailed.AddDynamic(this, &UHorrorUI::OnGrabFailed);
}

void UHorrorUI::OnSprintMeterUpdated(float Percent)
{
	// call the BP handler
	BP_SprintMeterUpdated(Percent);
}

void UHorrorUI::OnSprintStateChanged(bool bSprinting)
{
	// call the BP handler
	BP_SprintStateChanged(bSprinting);
}

void UHorrorUI::OnBatteryMeterUpdated(float Percent)
{
	// call the BP handler
	BP_BatteryMeterUpdated(Percent);
}

void UHorrorUI::OnFlashlightStateChanged(bool bOn)
{
	// call the BP handler
	BP_FlashlightStateChanged(bOn);
}

void UHorrorUI::OnFearMeterUpdated(float FearLevel)
{
	// call the BP handler
	BP_FearMeterUpdated(FearLevel);
}

void UHorrorUI::OnHighFearStateChanged(bool bHighFear)
{
	// call the BP handler
	BP_HighFearStateChanged(bHighFear);
}

void UHorrorUI::OnLoreNoteCollected(FName NoteID, FText NoteText)
{
	// call the BP handler
	BP_LoreNoteCollected(NoteID, NoteText);
}

void UHorrorUI::OnGrabMeterUpdated(float Percent)
{
	// call the BP handler
	BP_GrabMeterUpdated(Percent);
}

void UHorrorUI::OnGrabStateChanged(bool bGrabbed)
{
	// call the BP handler
	BP_GrabStateChanged(bGrabbed);
}

void UHorrorUI::OnGrabFailed()
{
	// call the BP handler
	BP_GrabFailed();
}

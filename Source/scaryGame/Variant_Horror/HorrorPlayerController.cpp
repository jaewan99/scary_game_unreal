// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "scaryGameCameraManager.h"
#include "HorrorCharacter.h"
#include "HorrorUI.h"
#include "HorrorGameMode.h"
#include "scaryGame.h"
#include "Widgets/Input/SVirtualJoystick.h"

AHorrorPlayerController::AHorrorPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AscaryGameCameraManager::StaticClass();
}

void AHorrorPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AHorrorPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(aPawn))
		{
			// create the UI
			if (!HorrorUI)
			{
				HorrorUI = CreateWidget<UHorrorUI>(this, HorrorUIClass);
				HorrorUI->AddToPlayerScreen(0);
			}

			HorrorUI->SetupCharacter(HorrorCharacter);

			// reload the last checkpoint if this character fails to escape a grab
			HorrorCharacter->OnGrabFailed.AddUniqueDynamic(this, &AHorrorPlayerController::OnCharacterGrabFailed);

			// track this character's key collection toward the game mode's win condition
			if (AHorrorGameMode* HorrorGameMode = GetWorld()->GetAuthGameMode<AHorrorGameMode>())
			{
				HorrorGameMode->RegisterCharacterForObjectives(HorrorCharacter);
			}

			// continue from the last save the first time a character is possessed this session
			if (bAutoLoadOnPossess && !bHasAutoLoaded)
			{
				bHasAutoLoaded = true;
				HorrorCharacter->LoadProgress(HorrorCharacter->DefaultSaveSlotName, HorrorCharacter->DefaultSaveUserIndex);
			}
		}
	}

}

void AHorrorPlayerController::OnCharacterGrabFailed()
{
	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
	{
		HorrorCharacter->LoadProgress(HorrorCharacter->DefaultSaveSlotName, HorrorCharacter->DefaultSaveUserIndex);
	}
}

void AHorrorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

		if (ShouldUseTouchControls())
		{
			// spawn the mobile controls widget
			MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

			if (MobileControlsWidget)
			{
				// add the controls to the player screen
				MobileControlsWidget->AddToPlayerScreen(0);

			} else {

				UE_LOG(LogscaryGame, Error, TEXT("Could not spawn mobile controls widget."));

			}

		}
	}
}

bool AHorrorPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HorrorPlayerController.generated.h"

class UInputMappingContext;
class UHorrorUI;

/**
 *  Player Controller for a first person horror game
 *  Manages input mappings
 *  Manages UI
 */
UCLASS(abstract, config="Game")
class SCARYGAME_API AHorrorPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Horror|UI")
	TSubclassOf<UHorrorUI> HorrorUIClass;

	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UHorrorUI> HorrorUI;

public:

	/** Constructor */
	AHorrorPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** If true, automatically loads the possessed character's DefaultSaveSlotName (if it exists) the first time a character is possessed this session, letting the player continue where they left off */
	UPROPERTY(EditAnywhere, Category = "Save")
	bool bAutoLoadOnPossess = true;

	/** True once the auto-load-on-possess check has run, so it doesn't repeat on every (re-)possess */
	bool bHasAutoLoaded = false;

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Possessed pawn initialization */
	virtual void OnPossess(APawn* aPawn) override;

	/** Reloads the possessed character's last save when they fail to escape a grab, acting as a checkpoint respawn */
	UFUNCTION()
	void OnCharacterGrabFailed();

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};

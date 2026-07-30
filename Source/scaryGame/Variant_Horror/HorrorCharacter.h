// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "scaryGameCharacter.h"
#include "HorrorCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class ANoiseDecoyProjectile;
class ABearTrap;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSprintMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSprintStateChangedDelegate, bool, bSprinting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateBatteryMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlashlightStateChangedDelegate, bool, bOn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateFearMeterDelegate, float, FearLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighFearStateChangedDelegate, bool, bHighFear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLoreNoteCollectedDelegate, FName, NoteID, FText, NoteText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateGrabMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrabStateChangedDelegate, bool, bGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGrabFailedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKeyCollectedDelegate, FName, KeyID);

/**
 *  Simple first person horror character
 *  Provides stamina-based sprinting
 */
UCLASS(abstract)
class SCARYGAME_API AHorrorCharacter : public AscaryGameCharacter
{
	GENERATED_BODY()

	/** Player light source */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

	/** Modular item storage. Keys (and, later, other collectibles) are held here rather than in bespoke fields on the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

protected:

	/** Fire weapon input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;

	/** Toggle flashlight input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* FlashlightAction;

	/** Toggle crouch input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* CrouchAction;

	/** Throw noise decoy input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ThrowDecoyAction;

	/** Barricade door input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* BarricadeAction;

	/** Escape-grab-attempt input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* EscapeAction;

	/** Place bear trap input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* PlaceTrapAction;

	/** Quicksave input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* QuickSaveAction;

	/** Quickload input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* QuickLoadAction;

	/** If true, we're sprinting */
	bool bSprinting = false;

	/** Walk speed while crouching */
	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float CrouchSpeed = 150.0f;

	/** If true, the flashlight (spotlight) is currently on */
	bool bFlashlightOn = true;

	/** How long (in seconds) the flashlight can stay on before the battery runs out */
	UPROPERTY(EditAnywhere, Category = "Flashlight", meta = (ClampMin = "0.0", Units = "s"))
	float BatteryLife = 60.0f;

	/** Time interval for battery drain ticks */
	UPROPERTY(EditAnywhere, Category = "Flashlight", meta = (ClampMin = "0", ClampMax = "1", Units = "s"))
	float BatteryFixedTickTime = 0.25f;

	/** Current battery charge, in seconds remaining */
	float BatteryMeter = 0.0f;

	/** Battery drain tick timer */
	FTimerHandle BatteryTimer;

	/** If true, we're recovering stamina */
	bool bRecovering = false;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	/** Time interval for sprinting stamina ticks */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float SprintFixedTickTime = 0.03333f;

	/** Sprint stamina amount. Maxes at SprintTime */
	float SprintMeter = 0.0f;

	/** How long we can sprint for, in seconds */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float SprintTime = 3.0f;

	/** Walk speed while sprinting */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float SprintSpeed = 600.0f;

	/** Loudness (0-1) of the noise event reported to AI hearing while sprinting */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SprintNoiseLoudness = 1.0f;

	/** Max range (in cm) that AI can hear sprint noise from */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", Units = "cm"))
	float SprintNoiseRange = 2000.0f;

	/** Loudness (0-1) of the noise event reported to AI hearing while walking normally */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WalkNoiseLoudness = 0.4f;

	/** Max range (in cm) that AI can hear normal walking noise from */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", Units = "cm"))
	float WalkNoiseRange = 700.0f;

	/** Loudness (0-1) of the noise event reported to AI hearing while crouch-walking; 0 means crouching is silent */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CrouchNoiseLoudness = 0.0f;

	/** Max range (in cm) that AI can hear crouch-walking noise from */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", Units = "cm"))
	float CrouchNoiseRange = 0.0f;

	/** Minimum time (in seconds) between movement noise reports */
	UPROPERTY(EditAnywhere, Category = "Movement Noise", meta = (ClampMin = "0.0", Units = "s"))
	float MovementNoiseInterval = 0.5f;

	/** Time accumulated since the last movement noise report */
	float TimeSinceLastMovementNoise = 0.0f;

	/** Walk speed while recovering stamina */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float RecoveringWalkSpeed = 150.0f;

	/** Time it takes for the sprint meter to recover */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float RecoveryTime = 0.0f;

	/** Sprint tick timer */
	FTimerHandle SprintTimer;

	/** How far to trace when interacting */
	UPROPERTY(EditAnywhere, Category = "Interaction", meta = (ClampMin = "0.0", Units = "cm"))
	float InteractDistance = 250.0f;

	/** IDs of lore notes the player has collected */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<FName> CollectedNoteIDs;

	/** True while hiding in an AHidingSpot; blocks sprinting while active */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bIsHiding = false;

	/** Current fear level, 0 (calm) to 1 (terrified) */
	UPROPERTY(BlueprintReadOnly, Category = "Fear")
	float FearLevel = 0.0f;

	/** How far (in cm) an enemy needs to be to affect fear at all */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0.0", Units = "cm"))
	float FearSenseRadius = 2000.0f;

	/** Fear gained per second while an enemy is within FearSenseRadius but not in view */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0.0"))
	float FearGainRateNearby = 0.05f;

	/** Fear gained per second while an enemy is within FearSenseRadius and in view */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0.0"))
	float FearGainRateVisible = 0.2f;

	/** Fear lost per second while no enemy is within FearSenseRadius */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0.0"))
	float FearDecayRate = 0.1f;

	/** Fear level (0-1) at/above which bHighFear becomes true */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HighFearThreshold = 0.75f;

	/** True while FearLevel is at/above HighFearThreshold */
	bool bHighFear = false;

	/** Time interval for fear meter ticks */
	UPROPERTY(EditAnywhere, Category = "Fear", meta = (ClampMin = "0", ClampMax = "1", Units = "s"))
	float FearFixedTickTime = 0.5f;

	/** Fear meter tick timer */
	FTimerHandle FearTimer;

	/** Class of projectile spawned by DoThrowDecoy */
	UPROPERTY(EditAnywhere, Category = "Noise Decoy")
	TSubclassOf<ANoiseDecoyProjectile> NoiseDecoyClass;

	/** Throw speed (cm/s) imparted to spawned decoys */
	UPROPERTY(EditAnywhere, Category = "Noise Decoy", meta = (ClampMin = "0.0", Units = "cm/s"))
	float DecoyThrowSpeed = 1500.0f;

	/** Number of noise decoys currently carried */
	UPROPERTY(BlueprintReadOnly, Category = "Noise Decoy")
	int32 NoiseDecoyCount = 0;

	/** Number of barricade materials (planks, furniture, etc.) currently carried */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	int32 BarricadeMaterialCount = 0;

	/** Class of trap spawned by DoPlaceTrap */
	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<ABearTrap> BearTrapClass;

	/** Distance (in cm), in front of the character, that DoPlaceTrap tries to place a trap at */
	UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0", Units = "cm"))
	float PlaceTrapDistance = 150.0f;

	/** Number of bear traps currently carried */
	UPROPERTY(BlueprintReadOnly, Category = "Trap")
	int32 BearTrapCount = 0;

	/** True while being grabbed by an enemy; movement is disabled except for escape attempts */
	UPROPERTY(BlueprintReadOnly, Category = "Grab")
	bool bIsGrabbed = false;

	/** How many escape presses are needed to break free of a grab */
	UPROPERTY(EditAnywhere, Category = "Grab", meta = (ClampMin = "1"))
	int32 EscapePressesRequired = 5;

	/** How long (in seconds) the player has to reach EscapePressesRequired before the grab succeeds */
	UPROPERTY(EditAnywhere, Category = "Grab", meta = (ClampMin = "0.0", Units = "s"))
	float EscapeWindowDuration = 3.0f;

	/** Escape presses registered so far this grab */
	UPROPERTY(BlueprintReadOnly, Category = "Grab")
	int32 EscapePressesMade = 0;

	/** Timer for the current grab's escape window */
	FTimerHandle EscapeWindowTimer;

	/** Walk/sprint speed multiplier applied while bInjured (e.g. after escaping a grab) */
	UPROPERTY(EditAnywhere, Category = "Grab", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float InjuredSpeedMultiplier = 0.6f;

	/** True while suffering the post-escape speed penalty, until healed */
	UPROPERTY(BlueprintReadOnly, Category = "Grab")
	bool bInjured = false;

	/** How long (in seconds) after a grab ends the character can't be re-grabbed; prevents catch/escape/catch loops when still standing next to the enemy */
	UPROPERTY(EditAnywhere, Category = "Grab", meta = (ClampMin = "0.0", Units = "s"))
	float PostGrabImmunityDuration = 2.0f;

	/** True during the brief immunity window right after a grab ends */
	bool bGrabImmune = false;

	/** Timer for the current post-grab immunity window */
	FTimerHandle GrabImmunityTimer;

public:

	/** Delegate called when the sprint meter should be updated */
	FUpdateSprintMeterDelegate OnSprintMeterUpdated;

	/** Delegate called when we start and stop sprinting */
	FSprintStateChangedDelegate OnSprintStateChanged;

	/** Delegate called when the flashlight battery meter should be updated */
	FUpdateBatteryMeterDelegate OnBatteryMeterUpdated;

	/** Delegate called when the flashlight is switched on or off */
	FFlashlightStateChangedDelegate OnFlashlightStateChanged;

	/** Delegate called when the fear meter should be updated */
	FUpdateFearMeterDelegate OnFearMeterUpdated;

	/** Delegate called when FearLevel crosses HighFearThreshold in either direction */
	FHighFearStateChangedDelegate OnHighFearStateChanged;

	/** Delegate called when a new lore note is collected */
	FLoreNoteCollectedDelegate OnLoreNoteCollected;

	/** Delegate called when grab escape progress (0-1, fraction of EscapePressesRequired) updates */
	FUpdateGrabMeterDelegate OnGrabMeterUpdated;

	/** Delegate called when a grab starts or ends (escaped or window expired) */
	FGrabStateChangedDelegate OnGrabStateChanged;

	/** Delegate called when the escape window expires without breaking free */
	FGrabFailedDelegate OnGrabFailed;

	/** Delegate called when a new key is collected (not broadcast for a duplicate key) */
	FKeyCollectedDelegate OnKeyCollected;

protected:

	/** Constructor */
	AHorrorCharacter();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:

	/** Starts sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartSprint();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoEndSprint();

	/** Called while sprinting at a fixed time interval */
	void SprintFixedTick();

	/** Called when the Interact input action is triggered */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoInteract();

	/** Called when the Flashlight input action is triggered; toggles the spotlight on/off if there's battery left */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoToggleFlashlight();

	/** Called while the flashlight is on at a fixed time interval; drains the battery and forces the light off when empty */
	void BatteryFixedTick();

	/** Called when the Crouch input action is triggered; toggles crouching on/off */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoToggleCrouch();

	/** Called when the ThrowDecoy input action is triggered; throws a noise decoy if one is carried */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoThrowDecoy();

	/** Called when the Barricade input action is triggered; barricades the door being looked at, if any and if material is carried */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoBarricade();

	/** Reports a noise event sized to the character's current movement state (sprinting/walking/crouching) */
	void ReportMovementNoise();

	/** Called at a fixed time interval; updates FearLevel based on nearby/visible enemies */
	void FearFixedTick();

	/** Called when the PlaceTrap input action is triggered; places a bear trap in front of the character if one is carried */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoPlaceTrap();

	/** Called when the Escape input action is triggered; registers one escape attempt while grabbed */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoEscapeAttempt();

	/** Called when the QuickSave input action is triggered; saves to DefaultSaveSlotName */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoQuickSave();

	/** Called when the QuickLoad input action is triggered; loads from DefaultSaveSlotName */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoQuickLoad();

	/** Called when the escape window's timer expires without the player breaking free */
	void EscapeWindowExpired();

	/** Ends the current grab; if bEscaped, applies the injured speed penalty, otherwise broadcasts OnGrabFailed */
	void EndGrab(bool bEscaped);

	/** Clears bGrabImmune once the post-grab immunity window expires */
	void EndGrabImmunity();

	/** Returns WalkSpeed, reduced by InjuredSpeedMultiplier while bInjured */
	float GetEffectiveWalkSpeed() const;

	/** Returns SprintSpeed, reduced by InjuredSpeedMultiplier while bInjured */
	float GetEffectiveSprintSpeed() const;

public:

	/** Returns the player's modular inventory component */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetInventory() const { return Inventory; }

	/** Returns true if the player has collected the given key */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	bool HasKey(FName KeyID) const;

	/** Adds a key to the player's collected keys */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void AddKey(FName KeyID);

	/** Recharges the flashlight battery by the given number of seconds, up to BatteryLife */
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void AddBatteryCharge(float Seconds);

	/** Moves the character into a hiding pose at HideTransform and disables movement input; called by AHidingSpot */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EnterHiding(const FTransform& HideTransform);

	/** Restores the character to RestoreTransform and re-enables movement input; called by AHidingSpot */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ExitHiding(const FTransform& RestoreTransform);

	/** Returns true while the character is hiding */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	bool IsHiding() const { return bIsHiding; }

	/** Returns true if the player has collected the given lore note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	bool HasCollectedNote(FName NoteID) const;

	/** Adds a lore note to the player's collected notes and broadcasts OnLoreNoteCollected, if not already collected */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CollectLoreNote(FName NoteID, const FText& NoteText);

	/** Adds to the number of noise decoys carried */
	UFUNCTION(BlueprintCallable, Category = "Noise Decoy")
	void AddNoiseDecoy(int32 Amount = 1);

	/** Adds to the number of barricade materials carried */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void AddBarricadeMaterial(int32 Amount = 1);

	/** Returns true if the flashlight is currently switched on */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flashlight")
	bool IsFlashlightOn() const { return bFlashlightOn; }

	/** Adds to the number of bear traps carried */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void AddBearTrap(int32 Amount = 1);

	/** Puts the character into a grabbed state with an escape window; called by the AI controller on catching the player */
	UFUNCTION(BlueprintCallable, Category = "Grab")
	void BeginGrab(AActor* Grabber);

	/** Clears the injured speed penalty; called by a healing pickup */
	UFUNCTION(BlueprintCallable, Category = "Grab")
	void Heal();

	/** Returns true during the brief immunity window right after a grab ends */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grab")
	bool IsGrabImmune() const { return bGrabImmune; }

	/** Save slot used by DoQuickSave/DoQuickLoad, and by AHorrorPlayerController's auto-load-on-possess/grab-failed-respawn logic */
	UPROPERTY(EditAnywhere, Category = "Save")
	FString DefaultSaveSlotName = TEXT("HorrorSaveSlot");

	/** Save user index used alongside DefaultSaveSlotName */
	UPROPERTY(EditAnywhere, Category = "Save")
	int32 DefaultSaveUserIndex = 0;

	/** Saves collected keys/notes/decoys/barricade material/traps/battery/transform to SlotName */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveProgress(const FString& SlotName, int32 UserIndex = 0);

	/** Loads previously saved progress from SlotName; returns false if the slot doesn't exist */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadProgress(const FString& SlotName, int32 UserIndex = 0);

	/** Returns true if a save exists at SlotName; useful for showing a "Continue" option in a main menu */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Save")
	static bool DoesSaveExist(const FString& SlotName, int32 UserIndex = 0);

	/** Deletes the save at SlotName, if any; useful for a "New Game" option that shouldn't continue an old save */
	UFUNCTION(BlueprintCallable, Category = "Save")
	static void DeleteSave(const FString& SlotName, int32 UserIndex = 0);
};

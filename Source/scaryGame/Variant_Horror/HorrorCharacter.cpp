// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "CollisionQueryParams.h"
#include "IInteractable.h"
#include "Perception/AISense_Hearing.h"
#include "NoiseDecoyProjectile.h"
#include "InteractableDoor.h"
#include "EnemyCharacter.h"
#include "BearTrap.h"
#include "HorrorSaveGame.h"
#include "Kismet/GameplayStatics.h"

AHorrorCharacter::AHorrorCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// initialize sprint meter to max
	SprintMeter = SprintTime;

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = GetEffectiveWalkSpeed();

	// start the sprint tick timer
	GetWorld()->GetTimerManager().SetTimer(SprintTimer, this, &AHorrorCharacter::SprintFixedTick, SprintFixedTickTime, true);

	// initialize the flashlight battery to max and start draining it (only actually drains while the flashlight is on)
	BatteryMeter = BatteryLife;
	GetWorld()->GetTimerManager().SetTimer(BatteryTimer, this, &AHorrorCharacter::BatteryFixedTick, BatteryFixedTickTime, true);

	// allow crouching and set its walk speed
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

	// start the fear meter tick timer
	GetWorld()->GetTimerManager().SetTimer(FearTimer, this, &AHorrorCharacter::FearFixedTick, FearFixedTickTime, true);
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the sprint timer
	GetWorld()->GetTimerManager().ClearTimer(SprintTimer);

	// clear the battery timer
	GetWorld()->GetTimerManager().ClearTimer(BatteryTimer);

	// clear the fear timer
	GetWorld()->GetTimerManager().ClearTimer(FearTimer);

	// clear the grab escape/immunity timers
	GetWorld()->GetTimerManager().ClearTimer(EscapeWindowTimer);
	GetWorld()->GetTimerManager().ClearTimer(GrabImmunityTimer);
}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	{
		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// Sprinting
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndSprint);

			// Interacting
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoInteract);

			// Flashlight
			EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoToggleFlashlight);

			// Crouching
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoToggleCrouch);

			// Throwing noise decoys
			EnhancedInputComponent->BindAction(ThrowDecoyAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoThrowDecoy);

			// Barricading doors
			EnhancedInputComponent->BindAction(BarricadeAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoBarricade);

			// Escaping a grab
			EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoEscapeAttempt);

			// Placing bear traps
			EnhancedInputComponent->BindAction(PlaceTrapAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoPlaceTrap);

			// Quicksave/quickload
			EnhancedInputComponent->BindAction(QuickSaveAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoQuickSave);
			EnhancedInputComponent->BindAction(QuickLoadAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoQuickLoad);

		}
	}
}

void AHorrorCharacter::DoStartSprint()
{
	// can't sprint while hiding, crouching, or grabbed
	if (bIsHiding || bIsCrouched || bIsGrabbed)
	{
		return;
	}

	// set the sprinting flag
	bSprinting = true;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the sprint walk speed
		GetCharacterMovement()->MaxWalkSpeed = GetEffectiveSprintSpeed();

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(true);
	}

}

void AHorrorCharacter::DoEndSprint()
{
	// set the sprinting flag
	bSprinting = false;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the default walk speed
		GetCharacterMovement()->MaxWalkSpeed = GetEffectiveWalkSpeed();

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::SprintFixedTick()
{
	// are we out of recovery, still have stamina and are moving faster than our walk speed?
	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{

		// do we still have meter to burn?
		if (SprintMeter > 0.0f)
		{
			// update the sprint meter
			SprintMeter = FMath::Max(SprintMeter - SprintFixedTickTime, 0.0f);

			// have we run out of stamina?
			if (SprintMeter <= 0.0f)
			{
				// raise the recovering flag
				bRecovering = true;

				// set the recovering walk speed
				GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
			}
		}

	} else {

		// recover stamina
		SprintMeter = FMath::Min(SprintMeter + SprintFixedTickTime, SprintTime);

		if (SprintMeter >= SprintTime)
		{
			// lower the recovering flag
			bRecovering = false;

			// set the walk or sprint speed depending on whether the sprint button is down
			GetCharacterMovement()->MaxWalkSpeed = bSprinting ? GetEffectiveSprintSpeed() : GetEffectiveWalkSpeed();

			// update the sprint state depending on whether the button is down or not
			OnSprintStateChanged.Broadcast(bSprinting);
		}

	}

	// broadcast the sprint meter updated delegate
	OnSprintMeterUpdated.Broadcast(SprintMeter / SprintTime);

	// periodically let nearby AI hear the character moving, sized to how loud the current movement is
	TimeSinceLastMovementNoise += SprintFixedTickTime;
	if (TimeSinceLastMovementNoise >= MovementNoiseInterval)
	{
		TimeSinceLastMovementNoise = 0.0f;
		ReportMovementNoise();
	}

}

void AHorrorCharacter::ReportMovementNoise()
{
	if (GetVelocity().IsNearlyZero())
	{
		return;
	}

	float Loudness = WalkNoiseLoudness;
	float Range = WalkNoiseRange;

	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{
		Loudness = SprintNoiseLoudness;
		Range = SprintNoiseRange;
	}
	else if (bIsCrouched)
	{
		Loudness = CrouchNoiseLoudness;
		Range = CrouchNoiseRange;
	}

	if (Loudness <= 0.0f || Range <= 0.0f)
	{
		return;
	}

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this, Range);
}

void AHorrorCharacter::DoInteract()
{
	if (bIsGrabbed)
	{
		return;
	}

	const FVector TraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (GetFirstPersonCameraComponent()->GetForwardVector() * InteractDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements<UInteractable>())
		{
			IInteractable::Execute_Interact(HitActor, this);
		}
	}
}

void AHorrorCharacter::DoToggleFlashlight()
{
	// don't allow turning a dead flashlight back on
	if (!bFlashlightOn && BatteryMeter <= 0.0f)
	{
		return;
	}

	// flip the flashlight state
	bFlashlightOn = !bFlashlightOn;

	// show/hide the spotlight accordingly
	SpotLight->SetVisibility(bFlashlightOn);

	OnFlashlightStateChanged.Broadcast(bFlashlightOn);
}

void AHorrorCharacter::BatteryFixedTick()
{
	if (!bFlashlightOn || BatteryMeter <= 0.0f)
	{
		return;
	}

	BatteryMeter = FMath::Max(BatteryMeter - BatteryFixedTickTime, 0.0f);
	OnBatteryMeterUpdated.Broadcast(BatteryMeter / BatteryLife);

	if (BatteryMeter <= 0.0f)
	{
		// battery died; force the flashlight off
		bFlashlightOn = false;
		SpotLight->SetVisibility(false);
		OnFlashlightStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::AddBatteryCharge(float Seconds)
{
	BatteryMeter = FMath::Clamp(BatteryMeter + Seconds, 0.0f, BatteryLife);
	OnBatteryMeterUpdated.Broadcast(BatteryMeter / BatteryLife);
}

void AHorrorCharacter::DoToggleCrouch()
{
	// can't crouch while hiding or grabbed
	if (bIsHiding || bIsGrabbed)
	{
		return;
	}

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		// can't sprint while crouched
		if (bSprinting)
		{
			DoEndSprint();
		}

		Crouch();
	}
}

void AHorrorCharacter::DoThrowDecoy()
{
	if (NoiseDecoyCount <= 0 || !NoiseDecoyClass || bIsHiding || bIsGrabbed)
	{
		return;
	}

	UCameraComponent* Camera = GetFirstPersonCameraComponent();
	const FVector SpawnLocation = Camera->GetComponentLocation() + (Camera->GetForwardVector() * 50.0f);
	const FTransform SpawnTransform(Camera->GetComponentRotation(), SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	if (ANoiseDecoyProjectile* Decoy = GetWorld()->SpawnActor<ANoiseDecoyProjectile>(NoiseDecoyClass, SpawnTransform, SpawnParams))
	{
		Decoy->Launch(Camera->GetForwardVector() * DecoyThrowSpeed);
		--NoiseDecoyCount;
	}
}

void AHorrorCharacter::DoBarricade()
{
	if (BarricadeMaterialCount <= 0 || bIsGrabbed)
	{
		return;
	}

	const FVector TraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (GetFirstPersonCameraComponent()->GetForwardVector() * InteractDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (AInteractableDoor* Door = Cast<AInteractableDoor>(HitResult.GetActor()))
		{
			if (Door->BarricadeDoor(this))
			{
				--BarricadeMaterialCount;
			}
		}
	}
}

void AHorrorCharacter::FearFixedTick()
{
	bool bEnemyNearby = false;
	bool bEnemyVisible = false;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);

	const FVector TraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();

	for (AActor* EnemyActor : Enemies)
	{
		if (FVector::Dist(GetActorLocation(), EnemyActor->GetActorLocation()) > FearSenseRadius)
		{
			continue;
		}

		bEnemyNearby = true;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(EnemyActor);

		FHitResult HitResult;
		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, EnemyActor->GetActorLocation(), ECC_Visibility, QueryParams);
		if (!bBlocked)
		{
			bEnemyVisible = true;
			break;
		}
	}

	if (bEnemyVisible)
	{
		FearLevel = FMath::Min(FearLevel + (FearGainRateVisible * FearFixedTickTime), 1.0f);
	}
	else if (bEnemyNearby)
	{
		FearLevel = FMath::Min(FearLevel + (FearGainRateNearby * FearFixedTickTime), 1.0f);
	}
	else
	{
		FearLevel = FMath::Max(FearLevel - (FearDecayRate * FearFixedTickTime), 0.0f);
	}

	OnFearMeterUpdated.Broadcast(FearLevel);

	const bool bNewHighFear = FearLevel >= HighFearThreshold;
	if (bNewHighFear != bHighFear)
	{
		bHighFear = bNewHighFear;
		OnHighFearStateChanged.Broadcast(bHighFear);
	}
}

void AHorrorCharacter::DoPlaceTrap()
{
	if (BearTrapCount <= 0 || !BearTrapClass || bIsGrabbed)
	{
		return;
	}

	const FVector Forward = GetActorForwardVector();
	const FVector TraceStart = GetActorLocation() + (Forward * PlaceTrapDistance) + FVector(0.0f, 0.0f, 50.0f);
	const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 200.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return;
	}

	const FTransform SpawnTransform(GetActorRotation(), HitResult.Location);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	if (GetWorld()->SpawnActor<ABearTrap>(BearTrapClass, SpawnTransform, SpawnParams))
	{
		--BearTrapCount;
	}
}

void AHorrorCharacter::DoEscapeAttempt()
{
	if (!bIsGrabbed)
	{
		return;
	}

	++EscapePressesMade;
	OnGrabMeterUpdated.Broadcast(static_cast<float>(EscapePressesMade) / static_cast<float>(EscapePressesRequired));

	if (EscapePressesMade >= EscapePressesRequired)
	{
		GetWorld()->GetTimerManager().ClearTimer(EscapeWindowTimer);
		EndGrab(true);
	}
}

void AHorrorCharacter::EscapeWindowExpired()
{
	EndGrab(false);
}

void AHorrorCharacter::EndGrab(bool bEscaped)
{
	bIsGrabbed = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	OnGrabStateChanged.Broadcast(false);

	if (bEscaped)
	{
		bInjured = true;
		GetCharacterMovement()->MaxWalkSpeed = GetEffectiveWalkSpeed();
	}
	else
	{
		OnGrabFailed.Broadcast();
	}

	// brief immunity so the enemy standing right next to us can't immediately re-catch us
	bGrabImmune = true;
	GetWorld()->GetTimerManager().SetTimer(GrabImmunityTimer, this, &AHorrorCharacter::EndGrabImmunity, PostGrabImmunityDuration, false);
}

void AHorrorCharacter::EndGrabImmunity()
{
	bGrabImmune = false;
}

float AHorrorCharacter::GetEffectiveWalkSpeed() const
{
	return bInjured ? WalkSpeed * InjuredSpeedMultiplier : WalkSpeed;
}

float AHorrorCharacter::GetEffectiveSprintSpeed() const
{
	return bInjured ? SprintSpeed * InjuredSpeedMultiplier : SprintSpeed;
}

void AHorrorCharacter::DoQuickSave()
{
	SaveProgress(DefaultSaveSlotName, DefaultSaveUserIndex);
}

void AHorrorCharacter::DoQuickLoad()
{
	LoadProgress(DefaultSaveSlotName, DefaultSaveUserIndex);
}

void AHorrorCharacter::EnterHiding(const FTransform& HideTransform)
{
	if (bIsHiding)
	{
		return;
	}

	bIsHiding = true;

	if (bSprinting)
	{
		DoEndSprint();
	}

	SetActorTransform(HideTransform);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}

void AHorrorCharacter::ExitHiding(const FTransform& RestoreTransform)
{
	if (!bIsHiding)
	{
		return;
	}

	bIsHiding = false;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	SetActorTransform(RestoreTransform);
}

bool AHorrorCharacter::HasKey(FName KeyID) const
{
	return CollectedKeys.Contains(KeyID);
}

void AHorrorCharacter::AddKey(FName KeyID)
{
	if (CollectedKeys.Contains(KeyID))
	{
		return;
	}

	CollectedKeys.Add(KeyID);
	OnKeyCollected.Broadcast(KeyID);
}

bool AHorrorCharacter::HasCollectedNote(FName NoteID) const
{
	return CollectedNoteIDs.Contains(NoteID);
}

void AHorrorCharacter::CollectLoreNote(FName NoteID, const FText& NoteText)
{
	if (CollectedNoteIDs.Contains(NoteID))
	{
		return;
	}

	CollectedNoteIDs.Add(NoteID);
	OnLoreNoteCollected.Broadcast(NoteID, NoteText);
}

void AHorrorCharacter::AddNoiseDecoy(int32 Amount)
{
	NoiseDecoyCount += Amount;
}

void AHorrorCharacter::AddBarricadeMaterial(int32 Amount)
{
	BarricadeMaterialCount += Amount;
}

void AHorrorCharacter::AddBearTrap(int32 Amount)
{
	BearTrapCount += Amount;
}

void AHorrorCharacter::BeginGrab(AActor* Grabber)
{
	if (bIsGrabbed)
	{
		return;
	}

	bIsGrabbed = true;
	EscapePressesMade = 0;

	if (bSprinting)
	{
		DoEndSprint();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	OnGrabMeterUpdated.Broadcast(0.0f);
	OnGrabStateChanged.Broadcast(true);

	GetWorld()->GetTimerManager().SetTimer(EscapeWindowTimer, this, &AHorrorCharacter::EscapeWindowExpired, EscapeWindowDuration, false);
}

void AHorrorCharacter::Heal()
{
	if (!bInjured)
	{
		return;
	}

	bInjured = false;
	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? GetEffectiveSprintSpeed() : GetEffectiveWalkSpeed();
}

void AHorrorCharacter::SaveProgress(const FString& SlotName, int32 UserIndex)
{
	UHorrorSaveGame* SaveGameInstance = Cast<UHorrorSaveGame>(UGameplayStatics::CreateSaveGameObject(UHorrorSaveGame::StaticClass()));
	if (!SaveGameInstance)
	{
		return;
	}

	SaveGameInstance->CollectedKeys = CollectedKeys;
	SaveGameInstance->CollectedNoteIDs = CollectedNoteIDs;
	SaveGameInstance->NoiseDecoyCount = NoiseDecoyCount;
	SaveGameInstance->BarricadeMaterialCount = BarricadeMaterialCount;
	SaveGameInstance->BearTrapCount = BearTrapCount;
	SaveGameInstance->BatteryMeter = BatteryMeter;
	SaveGameInstance->bInjured = bInjured;
	SaveGameInstance->PlayerTransform = GetActorTransform();
	SaveGameInstance->LevelName = FName(*GetWorld()->GetMapName());

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, UserIndex);
}

bool AHorrorCharacter::LoadProgress(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return false;
	}

	UHorrorSaveGame* SaveGameInstance = Cast<UHorrorSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!SaveGameInstance)
	{
		return false;
	}

	CollectedKeys = SaveGameInstance->CollectedKeys;
	CollectedNoteIDs = SaveGameInstance->CollectedNoteIDs;
	NoiseDecoyCount = SaveGameInstance->NoiseDecoyCount;
	BarricadeMaterialCount = SaveGameInstance->BarricadeMaterialCount;
	BearTrapCount = SaveGameInstance->BearTrapCount;

	BatteryMeter = SaveGameInstance->BatteryMeter;
	OnBatteryMeterUpdated.Broadcast(BatteryMeter / BatteryLife);

	bInjured = SaveGameInstance->bInjured;
	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? GetEffectiveSprintSpeed() : GetEffectiveWalkSpeed();

	SetActorTransform(SaveGameInstance->PlayerTransform);

	return true;
}

bool AHorrorCharacter::DoesSaveExist(const FString& SlotName, int32 UserIndex)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

void AHorrorCharacter::DeleteSave(const FString& SlotName, int32 UserIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}

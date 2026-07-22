// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
	// AIControllerClass is left as the default here; assign a Blueprint child of
	// AEnemyAIController to it (and set AutoPossessAI) on your BP_EnemyCharacter,
	// since AEnemyAIController is abstract and can't be spawned directly.
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// applied here (rather than the constructor) so Blueprint-overridden ChaseSpeed values are respected
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void AEnemyCharacter::ApplySnare(float Duration)
{
	if (bSnared)
	{
		// already snared; just refresh the duration
		GetWorldTimerManager().ClearTimer(SnareTimer);
	}
	else
	{
		SavedSpeedBeforeSnare = GetCharacterMovement()->MaxWalkSpeed;
		bSnared = true;
	}

	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	GetWorldTimerManager().SetTimer(SnareTimer, this, &AEnemyCharacter::EndSnare, Duration, false);
}

void AEnemyCharacter::EndSnare()
{
	bSnared = false;
	GetCharacterMovement()->MaxWalkSpeed = SavedSpeedBeforeSnare;
}

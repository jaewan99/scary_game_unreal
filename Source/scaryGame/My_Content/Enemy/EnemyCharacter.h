// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

/**
 * A simple enemy that chases the player once AEnemyAIController spots them.
 * Subclass as a Blueprint to assign a mesh/animations.
 */
UCLASS(abstract)
class SCARYGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	/** Movement speed while chasing the player */
	UPROPERTY(EditAnywhere, Category = "Chase")
	float ChaseSpeed = 400.0f;

	/** Points this enemy walks between when not chasing or investigating a noise. Leave empty to stand idle. */
	UPROPERTY(EditAnywhere, Category = "Patrol")
	TArray<AActor*> PatrolPoints;

	/** If true, PatrolPoints are visited in array order and looped; if false, a random point is picked each time */
	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool bPatrolInOrder = true;

	/** How long (in seconds) to pause at each patrol point before moving to the next */
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "0.0", Units = "s"))
	float PatrolWaitTime = 2.0f;

	/** Called by the AI controller when it gets close enough to the player to catch them */
	UFUNCTION(BlueprintImplementableEvent, Category = "Chase")
	void OnCaughtPlayer(APawn* Player);

	/** True while snared (e.g. by a bear trap), unable to move until the snare's timer ends */
	UPROPERTY(BlueprintReadOnly, Category = "Chase")
	bool bSnared = false;

	/** Freezes the enemy in place for Duration seconds; refreshes the duration if already snared */
	UFUNCTION(BlueprintCallable, Category = "Chase")
	void ApplySnare(float Duration);

protected:
	virtual void BeginPlay() override;

	/** Movement speed saved when the snare began, restored once it ends */
	float SavedSpeedBeforeSnare = 0.0f;

	/** Timer for the current snare's duration */
	FTimerHandle SnareTimer;

	/** Restores movement speed once the snare's timer expires */
	void EndSnare();
};

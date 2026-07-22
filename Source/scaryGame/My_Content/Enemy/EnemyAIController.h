// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AEnemyCharacter;
struct FAIStimulus;
struct FAIRequestID;
struct FPathFollowingResult;

/**
 * Sees the player via sight perception and hears noise events (e.g. sprinting) via hearing
 * perception, chasing/investigating until the player is caught or lost. Patrols between the
 * possessed AEnemyCharacter's PatrolPoints whenever it isn't chasing or investigating.
 */
UCLASS(abstract)
class SCARYGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAISenseConfig_Hearing* HearingConfig;

	/** How far (in cm) the enemy can spot the player */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float SightRadius = 1500.0f;

	/** How far (in cm) the enemy keeps tracking someone it has already spotted before losing them */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float LoseSightRadius = 1800.0f;

	/** Field of view, in degrees, for spotting the player */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float PeripheralVisionAngleDegrees = 80.0f;

	/** How far (in cm) the enemy can hear noise events, e.g. sprinting or doors */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float HearingRange = 2500.0f;

	/** If a sensed AHorrorCharacter's flashlight is off, a sight stimulus beyond this range (in cm) is ignored as too dark to spot; set >= SightRadius to disable this reduction */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float FlashlightOffSightRange = 600.0f;

	/** How far (in cm) other enemies can hear this one start chasing/investigating, causing them to also investigate */
	UPROPERTY(EditAnywhere, Category = "Perception")
	float AlertRadius = 2000.0f;

	/** How close the enemy needs to get to the player to catch them */
	UPROPERTY(EditAnywhere, Category = "Chase")
	float CatchDistance = 100.0f;

	/** How often (in seconds) the chase path is refreshed */
	UPROPERTY(EditAnywhere, Category = "Chase")
	float ChaseTickInterval = 0.25f;

	/** Actor currently being chased; unset once sight of them is lost (including while investigating a heard noise) */
	TWeakObjectPtr<AActor> TargetActor;

	/** Last known/heard location of a target; chased to before giving up */
	FVector LastKnownLocation = FVector::ZeroVector;

	/** True while actively pursuing a seen target, or investigating its last known/heard location */
	bool bChasing = false;

	FTimerHandle ChaseTimer;

	/** Index into the possessed AEnemyCharacter's PatrolPoints currently being walked to */
	int32 CurrentPatrolIndex = -1;

	/** Set while waiting at a patrol point before moving to the next one */
	FTimerHandle PatrolWaitTimer;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Ticks the chase: paths toward the target (or its last known/heard location), catches or gives up as appropriate */
	void UpdateChase();

	/** Marks the controller as chasing/investigating and starts the chase timer if it isn't already running */
	void StartChasing();

	virtual void BeginPlay() override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/** Begins (or resumes) walking the possessed pawn's patrol route, if it has one */
	void StartPatrolling();

	/** Moves toward the next patrol point in sequence (or a random one, per bPatrolInOrder), if not chasing */
	void MoveToNextPatrolPoint();
};

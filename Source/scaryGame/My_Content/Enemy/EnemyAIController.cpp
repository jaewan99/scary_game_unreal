// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "HorrorCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);

	SetPerceptionComponent(*AIPerception);
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		// sight only cares about the player - ignore other enemies, physics props, etc.
		const APawn* SensedPawn = Cast<APawn>(Actor);
		if (!SensedPawn || !SensedPawn->IsPlayerControlled())
		{
			return;
		}

		if (Stimulus.WasSuccessfullySensed())
		{
			// a hidden or flashlight-off player is harder (or impossible) to spot
			if (const AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Actor))
			{
				if (HorrorCharacter->IsHiding())
				{
					// hidden characters can't be spotted, regardless of level geometry
					LastKnownLocation = Stimulus.StimulusLocation;
					TargetActor = nullptr;
					return;
				}

				if (!HorrorCharacter->IsFlashlightOn() && GetPawn())
				{
					const float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Stimulus.StimulusLocation);
					if (Distance > FlashlightOffSightRange)
					{
						LastKnownLocation = Stimulus.StimulusLocation;
						TargetActor = nullptr;
						return;
					}
				}
			}

			TargetActor = Actor;
			StartChasing();
		}
		else
		{
			LastKnownLocation = Stimulus.StimulusLocation;
			TargetActor = nullptr;
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// Only investigate a heard noise if not already actively chasing a seen target
		if (Stimulus.WasSuccessfullySensed() && !TargetActor.IsValid())
		{
			LastKnownLocation = Stimulus.StimulusLocation;
			StartChasing();
		}
	}
}

void AEnemyAIController::StartChasing()
{
	const bool bWasAlreadyChasing = bChasing;
	bChasing = true;
	GetWorldTimerManager().ClearTimer(PatrolWaitTimer);

	// on freshly starting a chase/investigation, let nearby enemies hear the commotion and investigate too
	if (!bWasAlreadyChasing && GetPawn())
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetPawn()->GetActorLocation(), 1.0f, GetPawn(), AlertRadius);
	}

	if (!GetWorldTimerManager().IsTimerActive(ChaseTimer))
	{
		GetWorldTimerManager().SetTimer(ChaseTimer, this, &AEnemyAIController::UpdateChase, ChaseTickInterval, true);
	}
}

void AEnemyAIController::UpdateChase()
{
	APawn* ControlledPawn = GetPawn();
	if (!bChasing || !ControlledPawn)
	{
		bChasing = false;
		GetWorldTimerManager().ClearTimer(ChaseTimer);
		return;
	}

	if (const AEnemyCharacter* SnaredCharacter = Cast<AEnemyCharacter>(ControlledPawn))
	{
		if (SnaredCharacter->bSnared)
		{
			// frozen by a trap; can't pursue or catch the player until it breaks free
			return;
		}
	}

	if (AActor* Target = TargetActor.Get())
	{
		const float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), Target->GetActorLocation());
		if (DistanceToTarget <= CatchDistance)
		{
			// can't re-catch a target that's still in its post-grab immunity window; keep pursuing instead
			const AHorrorCharacter* HorrorCharacterTarget = Cast<AHorrorCharacter>(Target);
			if (HorrorCharacterTarget && HorrorCharacterTarget->IsGrabImmune())
			{
				MoveToActor(Target, 50.0f);
				return;
			}

			bChasing = false;
			StopMovement();
			GetWorldTimerManager().ClearTimer(ChaseTimer);

			if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(ControlledPawn))
			{
				EnemyCharacter->OnCaughtPlayer(Cast<APawn>(Target));
			}

			if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(Target))
			{
				HorrorCharacter->BeginGrab(ControlledPawn);
			}
			return;
		}

		MoveToActor(Target, 50.0f);
	}
	else
	{
		const float DistanceToLastKnown = FVector::Dist(ControlledPawn->GetActorLocation(), LastKnownLocation);
		if (DistanceToLastKnown <= 50.0f)
		{
			// reached the last known/heard location and found nothing; give up and resume patrolling
			bChasing = false;
			GetWorldTimerManager().ClearTimer(ChaseTimer);
			MoveToNextPatrolPoint();
			return;
		}

		MoveToLocation(LastKnownLocation, 50.0f);
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	StartPatrolling();
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (bChasing || !Result.IsSuccess())
	{
		return;
	}

	const AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (EnemyCharacter && EnemyCharacter->PatrolPoints.Num() > 0)
	{
		GetWorldTimerManager().SetTimer(PatrolWaitTimer, this, &AEnemyAIController::MoveToNextPatrolPoint, EnemyCharacter->PatrolWaitTime, false);
	}
}

void AEnemyAIController::StartPatrolling()
{
	const AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter || EnemyCharacter->PatrolPoints.Num() == 0)
	{
		return;
	}

	MoveToNextPatrolPoint();
}

void AEnemyAIController::MoveToNextPatrolPoint()
{
	if (bChasing)
	{
		return;
	}

	const AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter || EnemyCharacter->PatrolPoints.Num() == 0)
	{
		return;
	}

	const TArray<AActor*>& PatrolPoints = EnemyCharacter->PatrolPoints;

	CurrentPatrolIndex = EnemyCharacter->bPatrolInOrder
		? (CurrentPatrolIndex + 1) % PatrolPoints.Num()
		: FMath::RandHelper(PatrolPoints.Num());

	if (AActor* NextPoint = PatrolPoints[CurrentPatrolIndex])
	{
		MoveToActor(NextPoint, 50.0f);
	}
}

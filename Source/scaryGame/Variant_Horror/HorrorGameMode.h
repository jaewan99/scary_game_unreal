// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HorrorGameMode.generated.h"

class AHorrorCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObjectivesCompleteDelegate);

/**
 *  Simple GameMode for a first person horror game
 */
UCLASS(abstract)
class SCARYGAME_API AHorrorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	AHorrorGameMode();

	/** Key IDs (matched against AHorrorCharacter::HasKey) that must all be collected to complete this level's main objective. Leave empty to disable the win check. */
	UPROPERTY(EditAnywhere, Category = "Objectives")
	TArray<FName> RequiredKeyIDs;

	/** True once every RequiredKeyIDs has been collected */
	UPROPERTY(BlueprintReadOnly, Category = "Objectives")
	bool bObjectivesComplete = false;

	/** Delegate broadcast once when every required key has been collected */
	FObjectivesCompleteDelegate OnObjectivesComplete;

	/** Starts tracking HorrorCharacter's key collection toward RequiredKeyIDs; called by AHorrorPlayerController::OnPossess */
	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void RegisterCharacterForObjectives(AHorrorCharacter* HorrorCharacter);

protected:

	/** Initialization */
	virtual void BeginPlay() override;

	/** Assigns a PlayerStart to a specific player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:

	/** Determines how many local players should be spawned on game start */
	UPROPERTY(EditDefaultsOnly, Category="Local Multiplayer", meta = (ClampMin = 1, ClampMax = 4))
	int32 NumberOfLocalPlayers = 1;

	/** Used to assign players to different PlayerStarts in the level */
	int32 CurrentPlayerStartAssignment = 0;

	/** The character currently being tracked toward RequiredKeyIDs */
	TWeakObjectPtr<AHorrorCharacter> TrackedCharacter;

	/** Bound to the tracked character's OnKeyCollected; re-checks RequiredKeyIDs on every new key */
	UFUNCTION()
	void OnAnyKeyCollected(FName KeyID);
};

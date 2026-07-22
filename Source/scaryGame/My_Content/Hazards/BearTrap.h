// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BearTrap.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AEnemyCharacter;

/**
 * An environmental trap that snares the first AEnemyCharacter to step into its trigger volume,
 * freezing them in place for SnareDuration seconds - a window for the player to escape a chase.
 * Can be placed in the level directly, or spawned at runtime by AHorrorCharacter::DoPlaceTrap.
 * Subclass as a Blueprint to assign a mesh.
 */
UCLASS()
class SCARYGAME_API ABearTrap : public AActor
{
	GENERATED_BODY()

public:
	ABearTrap();

	/** How long (in seconds) a snared enemy is frozen in place */
	UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0", Units = "s"))
	float SnareDuration = 4.0f;

	/** If true, the trap re-arms itself automatically after TriggerCooldown; if false it stays sprung until RemoveBarricade-style manual reset */
	UPROPERTY(EditAnywhere, Category = "Trap")
	bool bAutoReset = false;

	/** Delay (in seconds) before an auto-resetting trap re-arms itself after triggering */
	UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bAutoReset"))
	float TriggerCooldown = 5.0f;

	/** True while the trap is sprung (already triggered) and can't catch another enemy until reset */
	UPROPERTY(BlueprintReadOnly, Category = "Trap")
	bool bSprung = false;

	/** Re-arms the trap so it can trigger again */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void ResetTrap();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerVolume;

	FTimerHandle AutoResetTimer;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Passes control to Blueprint when the trap springs on an enemy (e.g. to play a snap sound/VFX) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void BP_OnSprung(AEnemyCharacter* SnaredEnemy);

	/** Passes control to Blueprint when the trap is reset */
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void BP_OnReset();
};

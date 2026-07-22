// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoiseDecoyProjectile.generated.h"

class UStaticMeshComponent;

/**
 * A thrown object that reports a hearing stimulus at its impact point, letting the player lure
 * a nearby enemy away from their actual position. Spawned by AHorrorCharacter::DoThrowDecoy, which
 * calls Launch to set its initial velocity. Subclass as a Blueprint to assign a mesh.
 */
UCLASS()
class SCARYGAME_API ANoiseDecoyProjectile : public AActor
{
	GENERATED_BODY()

public:
	ANoiseDecoyProjectile();

	/** Loudness (0-1) of the noise event reported to AI hearing on impact */
	UPROPERTY(EditAnywhere, Category = "Noise")
	float ImpactNoiseLoudness = 1.0f;

	/** Max range (in cm) the impact noise can be heard from */
	UPROPERTY(EditAnywhere, Category = "Noise", meta = (ClampMin = "0.0", Units = "cm"))
	float ImpactNoiseRange = 3000.0f;

	/** How long (in seconds) after impact the projectile waits before destroying itself */
	UPROPERTY(EditAnywhere, Category = "Noise", meta = (ClampMin = "0.0", Units = "s"))
	float DestroyDelay = 2.0f;

	/** Sets the projectile's initial physics velocity; call right after spawning it */
	UFUNCTION(BlueprintCallable, Category = "Noise")
	void Launch(const FVector& Velocity);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Passes control to Blueprint on impact (e.g. to play a sound/VFX), before the noise event is reported */
	UFUNCTION(BlueprintImplementableEvent, Category = "Noise")
	void BP_OnImpact();

private:
	/** True once this projectile has already reported its impact noise */
	bool bHasImpacted = false;

	void DestroySelf();
};

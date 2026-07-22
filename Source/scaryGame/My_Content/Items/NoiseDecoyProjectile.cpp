// Fill out your copyright notice in the Description page of Project Settings.

#include "NoiseDecoyProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"

ANoiseDecoyProjectile::ANoiseDecoyProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->OnComponentHit.AddDynamic(this, &ANoiseDecoyProjectile::OnMeshHit);
	SetRootComponent(Mesh);

	InitialLifeSpan = 10.0f;
}

void ANoiseDecoyProjectile::Launch(const FVector& Velocity)
{
	Mesh->SetPhysicsLinearVelocity(Velocity);
}

void ANoiseDecoyProjectile::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasImpacted)
	{
		return;
	}

	bHasImpacted = true;

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), ImpactNoiseLoudness, this, ImpactNoiseRange);
	BP_OnImpact();

	FTimerHandle DestroyTimer;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ANoiseDecoyProjectile::DestroySelf, DestroyDelay, false);
}

void ANoiseDecoyProjectile::DestroySelf()
{
	Destroy();
}

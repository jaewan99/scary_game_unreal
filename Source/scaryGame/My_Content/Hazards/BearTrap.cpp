// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Hazards/BearTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "My_Content/Enemy/EnemyCharacter.h"
#include "TimerManager.h"

ABearTrap::ABearTrap()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(Mesh);
	TriggerVolume->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	TriggerVolume->SetCollisionProfileName(FName("Trigger"));
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABearTrap::OnTriggerBeginOverlap);
}

void ABearTrap::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bSprung)
	{
		return;
	}

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	if (!Enemy)
	{
		return;
	}

	bSprung = true;
	Enemy->ApplySnare(SnareDuration);
	BP_OnSprung(Enemy);

	if (bAutoReset)
	{
		GetWorldTimerManager().SetTimer(AutoResetTimer, this, &ABearTrap::ResetTrap, TriggerCooldown, false);
	}
}

void ABearTrap::ResetTrap()
{
	bSprung = false;
	BP_OnReset();
}

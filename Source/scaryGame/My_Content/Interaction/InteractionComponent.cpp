// Fill out your copyright notice in the Description page of Project Settings.

#include "My_Content/Interaction/InteractionComponent.h"
#include "My_Content/Interaction/IInteractable.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InteractAction = nullptr;
	InteractMappingContext = nullptr;
	TraceDistance = 250.0f;
	TraceChannel = ECC_Visibility;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn || !OwningPawn->IsLocallyControlled())
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(OwningPawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (InteractMappingContext)
		{
			Subsystem->AddMappingContext(InteractMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &UInteractionComponent::OnInteractTriggered);
		}
	}
}

void UInteractionComponent::OnInteractTriggered(const FInputActionValue& Value)
{
	AActor* HitActor = TraceForInteractable();
	if (HitActor && HitActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(HitActor, GetOwner());
	}
}

AActor* UInteractionComponent::TraceForInteractable() const
{
	const APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		return nullptr;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(OwningPawn->GetController());
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return nullptr;
	}

	const FVector TraceStart = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FVector TraceEnd = TraceStart + (PlayerController->PlayerCameraManager->GetCameraRotation().Vector() * TraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel, QueryParams))
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

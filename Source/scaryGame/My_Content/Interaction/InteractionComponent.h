// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * Attach to any pawn to let it interact with actors implementing IInteractable
 * when the bound Interact input action (default: E key) is pressed.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

	/** Input action that triggers an interact attempt. Assign an action mapped to the E key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Input")
	UInputAction* InteractAction;

	/** Mapping context containing InteractAction; added to the owning player's input subsystem on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Input")
	UInputMappingContext* InteractMappingContext;

	/** How far (in cm) to trace when looking for something to interact with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float TraceDistance;

	/** Collision channel used when tracing for interactable actors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	/** Bound to InteractAction; traces for and interacts with whatever the owner is looking at. */
	void OnInteractTriggered(const FInputActionValue& Value);

	/** Traces from the owner's camera view and returns the first actor hit, or nullptr. */
	AActor* TraceForInteractable() const;
};




#include "Component/InteractionComponent.h"

#include "GameFramework/Character.h"
#include "Interface/Interactable.h"
#include "UObject/ScriptInterface.h"

#define ECC_Interactable ECC_GameTraceChannel7

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceToFindNearestInteractable();
}

void UInteractionComponent::TraceToFindNearestInteractable()
{
	if (!Owner->Controller) {
		return;
	}

	if (bIsInteracting) {
		return;
	}

	FHitResult Hit;
	TArray<FHitResult> Hits;
	FVector StartLocation = Cast<APlayerController>(Owner->Controller)->PlayerCameraManager->GetCameraLocation();
	FVector EndLocation = StartLocation + Owner->GetBaseAimRotation().Vector() * TraceLength;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	// Multi logic
	// GetWorld()->SweepMultiByChannel(Hits, StartLocation, EndLocation, FQuat::Identity, ECC_Interactable, FCollisionShape::MakeSphere(TraceWidth), Params);
	// GetWorld()->LineTraceMultiByChannel(Hits, StartLocation, EndLocation, ECC_Interactable, Params);
	// 	if (Hits.Num() != 0)
	// 	{
	// 		AActor* PrioritaryInteractableActor = Hits[0].GetActor();
	// 		for (FHitResult Hit : Hits)
	// 		{
	// 			if (IInteractable* Interactable = Cast<IInteractable>(Hit.GetActor())) {
	// 				if (Cast<IInteractable>(PrioritaryInteractableActor)->GetPriority() < Interactable->GetPriority()) {
	// 					PrioritaryInteractableActor = Hit.GetActor();
	// 				}
	// 			}
	// 		}
	// 
	// 		if (CurrentInteractionTarget != PrioritaryInteractableActor) {
	// 			OnNewInteractionTarget.Broadcast(PrioritaryInteractableActor, CurrentInteractionTarget);
	// 		}
	// 		CurrentInteractionTarget = PrioritaryInteractableActor;
	// 	}
	// 	else
	// 	{
	// 		if (IsValid(CurrentInteractionTarget)) {
	// 			OnNewInteractionTarget.Broadcast(nullptr, CurrentInteractionTarget);
	// 		}
	// 		CurrentInteractionTarget = nullptr;
	// 	}

	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Interactable, Params);
	if (Hit.bBlockingHit && IsValid(Hit.GetActor()) && Cast<IInteractable>(Hit.GetActor())) {
		if (TScriptInterface<IInteractable> NewInteractionTarget = TScriptInterface<IInteractable>(Hit.GetActor())) {
			if (CurrentInteractionTarget != NewInteractionTarget) {
				OnNewInteractionTarget.Broadcast(NewInteractionTarget, CurrentInteractionTarget);
				if (CurrentInteractionTarget) {
					CurrentInteractionTarget->StopHover(this);
				}
				NewInteractionTarget->Hover(this);
				//
				CurrentInteractionTarget = NewInteractionTarget;
			}
		}
	}
	else {
		if (CurrentInteractionTarget != nullptr) {
			OnNewInteractionTarget.Broadcast(nullptr, CurrentInteractionTarget);
			CurrentInteractionTarget->StopHover(this);
			//
			CurrentInteractionTarget = nullptr;
		}
	}
}

bool UInteractionComponent::TryInteract()
{
	if (bIsInteracting) {
		if (StopInteract()) {
			return true;
		}
		return false;
	}
	else {
		if (CurrentInteractionTarget && CurrentInteractionTarget->Interact(this)) {
			bIsInteracting = true;
			return true;
		}
		return false;
	}
}

bool UInteractionComponent::StopInteract()
{
	if (CurrentInteractionTarget->StopInteract(this)) {
		bIsInteracting = false;
		return true;
	}
	return false;
}

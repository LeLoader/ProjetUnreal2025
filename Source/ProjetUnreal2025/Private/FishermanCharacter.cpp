// Fill out your copyright notice in the Description page of Project Settings.


#include "FishermanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include <Interactable.h>

DEFINE_LOG_CATEGORY(LogFishermanCharacter);

#define ECC_Interactable ECC_GameTraceChannel2

AFishermanCharacter::AFishermanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFishermanCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AFishermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceToFindNearestInteractable();
}

void AFishermanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFishermanCharacter::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFishermanCharacter::Interact);

		// Use
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &AFishermanCharacter::Use);
	}
	else
	{
		UE_LOG(LogFishermanCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFishermanCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ControlsMappingContext, 0);
		}
	}
}

void AFishermanCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AFishermanCharacter::Interact(const FInputActionValue& Value)
{
	Cast<IInteractable>(CurrentInteractionTarget)->Interact(Cast<AFishermanCharacter>(GetOwner()));
}

void AFishermanCharacter::Use(const FInputActionValue& Value)
{

}

void AFishermanCharacter::TraceToFindNearestInteractable()
{
	FHitResult Hit;
	TArray<FHitResult> Hits;
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = StartLocation + GetOwner()->GetActorForwardVector() * TraceLength;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	// Sphere sweep
	// GetWorld()->SweepMultiByChannel(Hits, StartLocation, EndLocation, FQuat::Identity, ECC_Interactable, FCollisionShape::MakeSphere(TraceWidth), Params);
	// GetWorld()->LineTraceMultiByChannel(Hits, StartLocation, EndLocation, ECC_Interactable, Params);
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Interactable, Params);
	if (Hit.bBlockingHit) {
		// Remplacer pour single 
	}

	if (Hits.Num() != 0)
	{
		AActor* PrioritaryInteractableActor = Hits[0].GetActor();
		for (FHitResult Hit : Hits)
		{
			if (IInteractable* Interactable = Cast<IInteractable>(Hit.GetActor())) {
				if (Cast<IInteractable>(PrioritaryInteractableActor)->GetPriority() < Interactable->GetPriority()) {
					PrioritaryInteractableActor = Hit.GetActor();
				}
			}
		}

		if (CurrentInteractionTarget != PrioritaryInteractableActor) {
			OnNewInteractionTarget.Broadcast(PrioritaryInteractableActor, CurrentInteractionTarget);
		}
		CurrentInteractionTarget = PrioritaryInteractableActor;
	}
	else
	{
		if (IsValid(CurrentInteractionTarget)) {
			OnNewInteractionTarget.Broadcast(nullptr, CurrentInteractionTarget);
		}
		CurrentInteractionTarget = nullptr;
	}
}
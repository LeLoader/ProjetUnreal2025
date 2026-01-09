// Fill out your copyright notice in the Description page of Project Settings.


#include "FishermanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include <Interactable.h>

DEFINE_LOG_CATEGORY(LogFishermanCharacter);

#define ECC_Interactable ECC_GameTraceChannel7

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

		// Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFishermanCharacter::Move);

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

	AddDefaultMappingContext();
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

void AFishermanCharacter::Move(const FInputActionValue& Value)
{

	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFishermanCharacter::Interact(const FInputActionValue& Value)
{
	if (IInteractable* InteractableObject = Cast<IInteractable>(CurrentInteractionTarget)) {
		if (bIsInteracting) {
			if (InteractableObject->StopInteract(this)) {
				AddDefaultMappingContext();
				bIsInteracting = false;
			}
		}
		else {
			if (InteractableObject->Interact(this)) {
				RemoveDefaultMappingContext();
				bIsInteracting = true;
			}
		}
	}
}

void AFishermanCharacter::StopInteract()
{
	AddDefaultMappingContext();
}

void AFishermanCharacter::Use(const FInputActionValue& Value)
{

}

void AFishermanCharacter::TraceToFindNearestInteractable()
{
	if (!Controller) {
		return;
	}

	if (bIsInteracting) {
		return;
	}

	FHitResult Hit;
	TArray<FHitResult> Hits;
	FVector StartLocation = Cast<APlayerController>(Controller)->PlayerCameraManager->GetCameraLocation();
	FVector EndLocation = StartLocation + GetBaseAimRotation().Vector() * TraceLength;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

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
	if (Hit.bBlockingHit && IsValid(Hit.GetActor())) {
		AActor* NewInteractionTarget = Hit.GetActor();
		if (CurrentInteractionTarget != NewInteractionTarget) {
			OnNewInteractionTarget.Broadcast(NewInteractionTarget, CurrentInteractionTarget);
			CurrentInteractionTarget = NewInteractionTarget;
		}
	}
	else {
		if (CurrentInteractionTarget != nullptr) {
			OnNewInteractionTarget.Broadcast(nullptr, CurrentInteractionTarget);
			CurrentInteractionTarget = nullptr;
		}
	}
}

void AFishermanCharacter::AddDefaultMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ControlsMappingContext, 0);
		}
	}
}

void AFishermanCharacter::RemoveDefaultMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = true;
			Subsystem->RemoveMappingContext(ControlsMappingContext, Options);
		}
	}
}

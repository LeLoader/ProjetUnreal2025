// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/FishermanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include "Interface/Interactable.h"
#include "Component/InteractionComponent.h"

DEFINE_LOG_CATEGORY(LogFishermanCharacter);

#define ECC_Interactable ECC_GameTraceChannel7

AFishermanCharacter::AFishermanCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AFishermanCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractionComponent->Owner = this;
}

void AFishermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (InteractionComponent->TryInteract()) {
		if (InteractionComponent->bIsInteracting) {
			RemoveDefaultMappingContext();
		}
		else {
			AddDefaultMappingContext();
		}
	}

// 	if (IInteractable* InteractableObject = Cast<IInteractable>(CurrentInteractionTarget)) {
// 		if (bIsInteracting) {
// 			if (InteractableObject->StopInteract(this)) {
// 				AddDefaultMappingContext();
// 				bIsInteracting = false;
// 			}
// 		}
// 		else {
// 			if (InteractableObject->Interact(this)) {
// 				RemoveDefaultMappingContext();
// 				bIsInteracting = true;
// 			}
// 		}
// 	}
}

void AFishermanCharacter::Use(const FInputActionValue& Value)
{

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

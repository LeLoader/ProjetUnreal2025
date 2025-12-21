// Fill out your copyright notice in the Description page of Project Settings.


#include "FishermanCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputTriggers.h"

DEFINE_LOG_CATEGORY(LogFishermanCharacter);

// Sets default values
AFishermanCharacter::AFishermanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFishermanCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFishermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFishermanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
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

	// Add Input Mapping Context
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
	// input is a Vector2D
	//	FVector2D LookAxisVector = Value.Get<FVector2D>();
	//	AController* Controller = GetController();
	//	if (IsValid(Controller)) {
	//	}
}

void AFishermanCharacter::Interact(const FInputActionValue& Value)
{
	//	FHitResult Hit;
	//	FVector TraceStart = GetActorLocation();
	//	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1000.0f;
	//	FCollisionQueryParams QueryParams;
	//	QueryParams.AddIgnoredActor(this);
	//	
	//	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);
	//	
	//	if (Hit.bBlockingHit) {
	//		if (IInteractable* Interactable = Cast<IInteractable>(Hit.GetActor())) {
	//			Interactable->Interact(this);
	//		}
	//	}
}

void AFishermanCharacter::Use(const FInputActionValue& Value)
{

}
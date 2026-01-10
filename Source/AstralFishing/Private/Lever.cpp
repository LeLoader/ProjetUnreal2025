


#include "Lever.h"

#include "FishermanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/StructuredLog.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ALever::ALever()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!IsValid(InputComponent)) {
		InputComponent = CreateDefaultSubobject<UEnhancedInputComponent>(TEXT("EnhancedInputComponent"));
	}

	LeverBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverBase"));
	RootComponent = LeverBase;

	Lever = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever"));
	Lever->SetupAttachment(RootComponent);

	FRotator BaseRotation = Lever->GetRelativeRotation();
	BaseRotation.Roll = 0;
	Lever->SetRelativeRotation(BaseRotation);
	CurrentDirection = ELeverDirection::MIDDLE;
}

// Called when the game starts or when spawned
void ALever::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveLeverAction, ETriggerEvent::Triggered, this, &ALever::MoveLever);
	}
}

bool ALever::Interact(AFishermanCharacter* InteractionSource)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->Controller))
	{
		if (!InputComponent) {
			return false;
		}

		PlayerController->PushInputComponent(InputComponent);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(LeverMappingContext, 1);
			return true;
		}
	}
	return false;
}

bool ALever::StopInteract(AActor* InteractionSource)
{
	AFishermanCharacter* FishermanCharacter = Cast<AFishermanCharacter>(InteractionSource);
	if (!IsValid(FishermanCharacter)) {
		FishermanCharacter = Cast<AFishermanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(FishermanCharacter->Controller)) {
		if (!InputComponent) {
			return false;
		}

		FishermanCharacter->StopInteract();
		PlayerController->PopInputComponent(InputComponent);

		// Needed if we want the lever to snap back in place
		/* 
		switch (CurrentDirection) {
		case ELeverDirection::MIDDLE:
			Lever->SetRelativeRotation(FRotator(Lever->GetRelativeRotation().Pitch, Lever->GetRelativeRotation().Yaw, 0));
			break;
		case ELeverDirection::LEFT:
			Lever->SetRelativeRotation(FRotator(Lever->GetRelativeRotation().Pitch, Lever->GetRelativeRotation().Yaw, -MaxAngle));
			break;
		case ELeverDirection::RIGHT:
			Lever->SetRelativeRotation(FRotator(Lever->GetRelativeRotation().Pitch, Lever->GetRelativeRotation().Yaw, MaxAngle));
			break;
		}
		*/

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = true;
			Options.bForceImmediately = false;
			Subsystem->RemoveMappingContext(LeverMappingContext, Options);
			return true;

		}
	}
	return false;
}

void ALever::SetLeverDirection(ELeverDirection NewDirection)
{
	if (CurrentDirection == NewDirection) return; // Ignore if same direction
	ELeverDirection OldDirection = CurrentDirection;
	CurrentDirection = NewDirection;
	OnLeverDirectionChanged.Broadcast(NewDirection, OldDirection);

	switch (NewDirection) {
	case ELeverDirection::MIDDLE:
		if (IsValid(StopSound)) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), StopSound, GetActorLocation());
		}
		break;
	case ELeverDirection::LEFT:
	case ELeverDirection::RIGHT:
		if (IsValid(ClickSound)) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ClickSound, GetActorLocation());
		}
		break;
	}
}

void ALever::MoveLever(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();
	FRotator CurrentRotation = Lever->GetRelativeRotation();
	CurrentRotation.Add(0, 0, FloatValue);
	CurrentRotation.Roll = FMath::Clamp(CurrentRotation.Roll, -MaxAngle, MaxAngle);

	float ThirdOfMaxAngleRange = MaxAngle * 2 / 3;

	if (CurrentRotation.Roll <= -MaxAngle + ThirdOfMaxAngleRange) {
		SetLeverDirection(ELeverDirection::LEFT);
		// StopInteract(this);
	}
	else if (CurrentRotation.Roll >= MaxAngle - ThirdOfMaxAngleRange) {
		SetLeverDirection(ELeverDirection::RIGHT);
		// StopInteract(this);
	}
	else if (CurrentRotation.Roll >= -MaxAngle + ThirdOfMaxAngleRange && CurrentRotation.Roll <= MaxAngle - ThirdOfMaxAngleRange) {
		SetLeverDirection(ELeverDirection::MIDDLE);
	}

	Lever->SetRelativeRotation(CurrentRotation);
}
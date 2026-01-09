


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

	FRotator Rot = Lever->GetRelativeRotation();
	Rot.Roll = 0;
	Lever->SetRelativeRotation(Rot);
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
}

void ALever::MoveLever(const FInputActionValue& Value)
{
	float FloatValue = Value.Get<float>();
	UE_LOGFMT(LogFishermanCharacter, Display, "Move Lever {0}", FloatValue);

	FRotator CurrentRotation = Lever->GetRelativeRotation();
	CurrentRotation.Add(0, 0, FloatValue);
	CurrentRotation.Roll = FMath::Clamp(CurrentRotation.Roll, -MaxAngle, MaxAngle);
	if (CurrentRotation.Roll == -MaxAngle) {
		SetLeverDirection(ELeverDirection::LEFT);
		StopInteract(this);
	}
	else if (CurrentRotation.Roll == MaxAngle) {
		SetLeverDirection(ELeverDirection::RIGHT);
		StopInteract(this);
	}
	else if (CurrentDirection == ELeverDirection::LEFT && CurrentRotation.Roll >= 0 || CurrentDirection == ELeverDirection::RIGHT && CurrentRotation.Roll <= 0) {
		SetLeverDirection(ELeverDirection::MIDDLE);
	}

	Lever->SetRelativeRotation(CurrentRotation);
}
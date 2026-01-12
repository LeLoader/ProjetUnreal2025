


#include "Gameplay/Lever.h"

#include "Global/FishermanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/StructuredLog.h"
#include <Kismet/GameplayStatics.h>
#include "Component/InteractionComponent.h"

// Sets default values
ALever::ALever()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!IsValid(InputComponent)) {
		InputComponent = CreateDefaultSubobject<UEnhancedInputComponent>(TEXT("EnhancedInputComponent"));
	}

	LeverBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM LeverBase"));
	RootComponent = LeverBaseMesh;
	ToOutline.Add(LeverBaseMesh);

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Lever"));
	LeverMesh->SetupAttachment(RootComponent);
	ToOutline.Add(LeverMesh);

	FRotator BaseRotation = LeverMesh->GetRelativeRotation();
	BaseRotation.Roll = 0;
	LeverMesh->SetRelativeRotation(BaseRotation);
	WantedRotation = BaseRotation;
	CurrentDirection = ELeverDirection::MIDDLE;
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveLeverAction, ETriggerEvent::Triggered, this, &ALever::MoveLever);
	}
}

void ALever::Tick(float DeltaTime)
{
	LeverMesh->SetRelativeRotation(FMath::RInterpTo(LeverMesh->GetRelativeRotation(), WantedRotation, DeltaTime, SnapSpeed));
}

bool ALever::Interact(UInteractionComponent* InteractionSource)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->Owner->Controller))
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

bool ALever::StopInteract(UInteractionComponent* InteractionSource)
{
	AFishermanCharacter* FishermanCharacter = Cast<AFishermanCharacter>(InteractionSource->Owner);
	if (!IsValid(FishermanCharacter)) {
		FishermanCharacter = Cast<AFishermanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(FishermanCharacter->Controller)) {
		if (!InputComponent) {
			return false;
		}

		PlayerController->PopInputComponent(InputComponent);

		switch (CurrentDirection) {
		case ELeverDirection::MIDDLE:
			DirectionValue = 0;
			break;
		case ELeverDirection::LEFT:
			DirectionValue = -1;
			break;
		case ELeverDirection::RIGHT:
			DirectionValue = 1;
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
	DirectionValue = FMath::Clamp(DirectionValue + FloatValue * LeverSensitivity, -1, 1);

	if (DirectionValue <= -1) {
		SetLeverDirection(ELeverDirection::LEFT);
		WantedRotation.Roll = -MaxAngle;
	}
	else if (DirectionValue >= 1) {
		SetLeverDirection(ELeverDirection::RIGHT);
		WantedRotation.Roll = MaxAngle;
	}
	else if ((DirectionValue >= 0 && CurrentDirection == ELeverDirection::LEFT) ||  (DirectionValue <= 0 && CurrentDirection == ELeverDirection::RIGHT)) {
		SetLeverDirection(ELeverDirection::MIDDLE);
		WantedRotation.Roll = 0;
	}
}
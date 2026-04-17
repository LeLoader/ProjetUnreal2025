


#include "Gameplay/Lever.h"

#include "Global/FishermanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/StructuredLog.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ALever::ALever()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsToggleInteraction = true;

	if (!IsValid(InputComponent)) {
		InputComponent = CreateDefaultSubobject<UEnhancedInputComponent>(TEXT("EnhancedInputComponent"));
	}

	LeverBaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM LeverBase"));
	RootComponent = LeverBaseMeshComponent;
	LeverBaseMeshComponent->SetCollisionProfileName(InteractableProfileStatic);
	ToOutline.Add(LeverBaseMeshComponent);

	LeverMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Lever"));
	LeverMeshComponent->SetupAttachment(LeverBaseMeshComponent);
	LeverMeshComponent->SetCollisionProfileName(InteractableProfileDynamic);
	ToOutline.Add(LeverMeshComponent);

	FRotator BaseRotation = LeverMeshComponent->GetRelativeRotation();
	BaseRotation.Roll = 0;
	LeverMeshComponent->SetRelativeRotation(BaseRotation);
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
	if (bIsInteracting || !bReachedWantedRotation) {
		
		LeverMeshComponent->SetRelativeRotation(FMath::RInterpTo(LeverMeshComponent->GetRelativeRotation(), WantedRotation, DeltaTime, SnapSpeed));
		bReachedWantedRotation = WantedRotation.Equals(LeverMeshComponent->GetRelativeRotation(), 0.1f);
	}
}

FInteractionResult ALever::Interact(UInteractionComponent* InteractionSource)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->OwningCharacter->Controller))
	{
		if (!InputComponent) {
			return FInteractionResult(false, bIsToggleInteraction);
		}

		PlayerController->PushInputComponent(InputComponent);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(LeverMappingContext, 1);
			bIsInteracting = true;
			return FInteractionResult(true, bIsToggleInteraction);
		}
	}
	return FInteractionResult(false, bIsToggleInteraction);
}

bool ALever::StopInteract(UInteractionComponent* InteractionSource)
{
	AFishermanCharacter* FishermanCharacter = Cast<AFishermanCharacter>(InteractionSource->OwningCharacter);
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
			bIsInteracting = false;
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
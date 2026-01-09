


#include "Lever.h"

#include "FishermanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/StructuredLog.h"

// Sets default values
ALever::ALever()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!IsValid(InputComponent)) {
		InputComponent = CreateDefaultSubobject<UEnhancedInputComponent>(TEXT("EnhancedInputComponent"));
	}
}

// Called when the game starts or when spawned
void ALever::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

		EnhancedInputComponent->BindAction(MoveLeverAction, ETriggerEvent::Triggered, this, &ALever::MoveLever);
	}
}

void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALever::Interact(AFishermanCharacter* InteractionSource)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->Controller))
	{
		if (!InputComponent) {
			return false;
		}

		PlayerController->PushInputComponent(InputComponent);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(LeverMappingContext, 1);
			return true;
		}
	}
	return false;
}

bool ALever::StopInteract(AFishermanCharacter* InteractionSource)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->Controller))
	{
		if (!InputComponent) 
		{ 
			return false;
		}

		PlayerController->PopInputComponent(InputComponent);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = true;
			Options.bForceImmediately = false;
			Subsystem->RemoveMappingContext(LeverMappingContext, Options);
			return true;
		}
	}
	return false;
}

void ALever::MoveLever(const FInputActionValue& Value)
{
	UE_LOGFMT(LogFishermanCharacter, Display, "Move Lever {0}", Value.Get<float>());
}
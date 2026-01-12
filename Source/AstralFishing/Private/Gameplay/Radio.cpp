


#include "Gameplay/Radio.h"

#include "Components/AudioComponent.h"
#include "AudioParameterControllerInterface.h"

ARadio::ARadio()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsToggleInteraction = false;

	RadioMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Radio"));
	RadioMeshComponent->SetCollisionProfileName(InteractableProfileStatic);
	RootComponent = RadioMeshComponent;
	ToOutline.Add(RadioMeshComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	AudioComponent->SetupAttachment(RadioMeshComponent);
}

void ARadio::BeginPlay()
{
	Super::BeginPlay();

	if (Music) {
		AudioComponent->SetSound(Music);
		AudioComponent->Play();
	}
}

FInteractionResult ARadio::Interact(UInteractionComponent* InteractionSource)
{
	if (AudioComponent) {
		AudioComponent->SetTriggerParameter(FName("NextSound"));
		return FInteractionResult(true, bIsToggleInteraction);
	}
	return FInteractionResult(false, bIsToggleInteraction);
}

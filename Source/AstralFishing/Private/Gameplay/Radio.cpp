


#include "Gameplay/Radio.h"

#include "Components/AudioComponent.h"
#include "AudioParameterControllerInterface.h"

ARadio::ARadio()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsToggleInteraction = false;

	RadioMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Radio"));
	RootComponent = RadioMesh;
	ToOutline.Add(RadioMesh);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	AudioComponent->SetupAttachment(RadioMesh);
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

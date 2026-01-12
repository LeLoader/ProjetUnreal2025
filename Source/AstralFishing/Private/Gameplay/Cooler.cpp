


#include "Gameplay\Cooler.h"

#include "Components/Widget.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACooler::ACooler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsToggleInteraction = true;
}

// Called when the game starts or when spawned
void ACooler::BeginPlay()
{
	Super::BeginPlay();
	
}

FInteractionResult ACooler::Interact(UInteractionComponent* InteractionSource)
{
	if (WidgetClass != nullptr) {
		Widget = CreateWidget(GetWorld(), WidgetClass, TEXT("Widget"));
		Widget->AddToViewport();
		return FInteractionResult(true, bIsToggleInteraction);
	}
	return FInteractionResult(false, bIsToggleInteraction);
}

bool ACooler::StopInteract(UInteractionComponent* InteractionSource)
{
	Widget->RemoveFromParent();
	Widget = nullptr;
	return true;
}

// Called every frame
void ACooler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



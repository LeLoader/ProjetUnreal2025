


#include "Gameplay\Cooler.h"

#include "Components/Widget.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "UnrealClient.h"

// Sets default values
ACooler::ACooler()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsToggleInteraction = true;

	CoolerBodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Cooler Body"));
	RootComponent = CoolerBodyMeshComponent;
	CoolerBodyMeshComponent->SetCollisionProfileName(InteractableProfileStatic);
	ToOutline.Add(CoolerBodyMeshComponent);

	CoolerLidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Cooler Lid"));
	CoolerLidMeshComponent->SetupAttachment(CoolerBodyMeshComponent);
	CoolerLidMeshComponent->SetCollisionProfileName(InteractableProfileDynamic);
	ToOutline.Add(CoolerLidMeshComponent);
}

void ACooler::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass != nullptr) {
		Widget = CreateWidget(GetWorld(), WidgetClass, TEXT("WidgetCooler"));
	}
}

void ACooler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FInteractionResult ACooler::Interact(UInteractionComponent* InteractionSource)
{
	if (!Widget) return FInteractionResult(false, bIsToggleInteraction);

	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->OwningCharacter->Controller)) {
		PlayerController->SetShowMouseCursor(true);
		FIntPoint ViewportSize = GEditor->GetActiveViewport()->GetSizeXY();
		PlayerController->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2); // Assumed this is the center
		// PlayerController->SetInputMode(FInputModeUIOnly());
		Widget->AddToViewport();
		return FInteractionResult(true, bIsToggleInteraction);
	}
	return FInteractionResult(false, bIsToggleInteraction);
}

bool ACooler::StopInteract(UInteractionComponent* InteractionSource)
{
	if (!Widget) return false;

	if (APlayerController* PlayerController = Cast<APlayerController>(InteractionSource->OwningCharacter->Controller)) {
		PlayerController->SetShowMouseCursor(false);
		// PlayerController->SetInputMode(FInputModeGameAndUI());
		Widget->RemoveFromParent();
		return true;
	}
	return false;
}

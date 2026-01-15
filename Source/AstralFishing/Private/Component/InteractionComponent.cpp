


#include "Component/InteractionComponent.h"

#include "Interface/Interactable.h"
#include "UObject/ScriptInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Global/FishermanCharacter.h"
#include "Camera/CameraComponent.h"

#define ECC_Interactable ECC_GameTraceChannel7

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UInteractionComponent::OnRegister()
{
	Super::OnRegister();

	OwningCharacter = Cast<ACharacter>(GetOwner());
}


void UInteractionComponent::OnUnregister()
{
	Super::OnUnregister();

	if (OwningCharacter) {
		if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(InteractionMappingContext);
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(OwningCharacter->InputComponent)) {
			EnhancedInputComponent->RemoveActionBindingForHandle(InteractStartedHandle);
			EnhancedInputComponent->RemoveActionBindingForHandle(UseStartedHandle);
		}

		OwningCharacter = nullptr;
	}
}

void UInteractionComponent::SetupInputs()
{
	if (OwningCharacter) {

		if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(InteractionMappingContext, 0);
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(OwningCharacter->InputComponent)) {
			InteractStartedHandle = EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UInteractionComponent::TryInteract).GetHandle();
			UseStartedHandle = EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &UInteractionComponent::TryUse).GetHandle();
		}
	}
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceToFindNearestInteractable();
}

void UInteractionComponent::TraceToFindNearestInteractable()
{
	if (!OwningCharacter->Controller) {
		return;
	}

	if (bIsInteracting) {
		return;
	}

	FHitResult Hit;
	TArray<FHitResult> Hits;
	FVector StartLocation = Cast<AFishermanCharacter>(OwningCharacter)->GetFirstPersonCamera()->GetComponentTransform().GetLocation();
	FVector EndLocation = StartLocation + OwningCharacter->GetBaseAimRotation().Vector() * TraceLength;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwningCharacter);

	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Interactable, Params);
	if (Hit.bBlockingHit && IsValid(Hit.GetActor())) {
		if (TScriptInterface<IInteractable> NewInteractionTarget = TScriptInterface<IInteractable>(Hit.GetActor())) {
			if (CurrentInteractionTarget != NewInteractionTarget) {
				OnNewInteractionTarget.Broadcast(NewInteractionTarget, CurrentInteractionTarget);
				if (CurrentInteractionTarget) {
					CurrentInteractionTarget->StopHover(this);
				}
				NewInteractionTarget->Hover(this);
				//
				CurrentInteractionTarget = NewInteractionTarget;
			}
			return;
		}
	}


	if (CurrentInteractionTarget != nullptr) {
		OnNewInteractionTarget.Broadcast(nullptr, CurrentInteractionTarget);
		CurrentInteractionTarget->StopHover(this);
		//
		CurrentInteractionTarget = nullptr;
	}
}

void UInteractionComponent::TryInteract()
{
	if (bIsInteracting) {
		StopInteract();
		Cast<AFishermanCharacter>(OwningCharacter)->AddDefaultMappingContext();
	}
	else {
		if (CurrentInteractionTarget) {
			FInteractionResult Result = CurrentInteractionTarget->Interact(this);
			if (Result.bHasSuccess && Result.bIsToggleInteraction) {
				bIsInteracting = true;
				Cast<AFishermanCharacter>(OwningCharacter)->RemoveDefaultMappingContext();
			}
		}
	}
}

bool UInteractionComponent::StopInteract()
{
	if (CurrentInteractionTarget->StopInteract(this)) {
		bIsInteracting = false;
		return true;
	}
	return false;
}

void UInteractionComponent::TryUse()
{

}

bool UInteractionComponent::StopUse()
{
	return true;
}

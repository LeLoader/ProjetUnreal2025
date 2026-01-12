


#include "Gameplay/InteractableBase.h"

FInteractionResult AInteractableBase::Interact(UInteractionComponent* InteractionSource)
{
	return FInteractionResult(true, bIsToggleInteraction);
}

void AInteractableBase::Hover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutline) {
		Comp->CustomDepthStencilValue = 1;
		Comp->SetRenderCustomDepth(true);
	}
}

void AInteractableBase::StopHover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutline) {
		Comp->SetRenderCustomDepth(false);
	}
}



#include "Gameplay/InteractableBase.h"

bool AInteractableBase::Interact(UInteractionComponent* InteractionSource)
{
	return true;
}

void AInteractableBase::Hover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutline) {
		Comp->SetRenderCustomDepth(true);
	}
}

void AInteractableBase::StopHover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutline) {
		Comp->SetRenderCustomDepth(false);
	}
}
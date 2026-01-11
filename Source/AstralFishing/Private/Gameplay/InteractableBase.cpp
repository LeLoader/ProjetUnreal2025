


#include "Gameplay/InteractableBase.h"

bool AInteractableBase::Interact(UInteractionComponent* InteractionSource)
{
	return true;
}

void AInteractableBase::Hover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutlines) {
		Comp->SetRenderCustomDepth(true);
	}
}

void AInteractableBase::StopHover(UInteractionComponent* InteractionSource)
{
	for (UPrimitiveComponent* Comp : ToOutlines) {
		Comp->SetRenderCustomDepth(false);
	}
}




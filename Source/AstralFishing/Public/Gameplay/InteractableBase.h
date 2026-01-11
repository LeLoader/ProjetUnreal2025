

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"

#include "InteractableBase.generated.h"

UCLASS()
class ASTRALFISHING_API AInteractableBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
#pragma region Interaction Implementation

public:
	virtual bool Interact(UInteractionComponent* InteractionSource) override;
	void Hover(UInteractionComponent* InteractionSource) override;
	void StopHover(UInteractionComponent* InteractionSource) override;

#pragma endregion Interaction Implementation

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UPrimitiveComponent>> ToOutlines;
};

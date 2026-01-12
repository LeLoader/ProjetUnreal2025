

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Interactable.generated.h"

class UInteractionComponent;

USTRUCT(BlueprintType)
struct FInteractionResult {

	GENERATED_BODY()

	bool bHasSuccess = false;
	bool bIsToggleInteraction = false;

};


// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASTRALFISHING_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual FInteractionResult Interact(UInteractionComponent* InteractionSource) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool StopInteract(UInteractionComponent* InteractionSource);

	UFUNCTION(BlueprintCallable)
	virtual void Hover(UInteractionComponent* InteractionSource) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void StopHover(UInteractionComponent* InteractionSource) = 0;

	// UFUNCTION(BlueprintCallable)
	// virtual int GetPriority() = 0;
	
};

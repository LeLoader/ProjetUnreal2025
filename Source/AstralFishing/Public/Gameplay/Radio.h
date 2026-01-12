

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Gameplay/InteractableBase.h"

#include "Radio.generated.h"

class UAudioComponent;

UCLASS()
class ASTRALFISHING_API ARadio : public AInteractableBase
{
	GENERATED_BODY()
	
public:	
	ARadio();

protected:
	virtual void BeginPlay() override;

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RadioMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> AudioComponent;

#pragma endregion Components

#pragma region Interaction Implementation

public:
	FInteractionResult Interact(UInteractionComponent* InteractionSource) override;

#pragma endregion Interaction Implementation

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Music;
};

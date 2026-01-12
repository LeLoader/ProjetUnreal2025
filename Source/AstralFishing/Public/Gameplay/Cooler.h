

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/InteractableBase.h"

#include "Cooler.generated.h"

UCLASS()
class ASTRALFISHING_API ACooler : public AInteractableBase
{
	GENERATED_BODY()
	
public:	
	ACooler();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> CoolerBodyMeshComponent;

	private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> CoolerLidMeshComponent;

#pragma endregion Components

#pragma region Interaction Implementation

public:
	FInteractionResult Interact(UInteractionComponent* InteractionSource) override;
	bool StopInteract(UInteractionComponent* InteractionSource) override;

#pragma endregion Interaction Implementation

public:
	UPROPERTY(EditDefaultsOnly);
	TSubclassOf<class UUserWidget> WidgetClass;

	UUserWidget* Widget;
};

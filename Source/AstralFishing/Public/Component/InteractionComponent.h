

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"

#include "InteractionComponent.generated.h"

class IInteractable;
class ACharacter;

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionTargetEvent, TScriptInterface<IInteractable>, NewInteractionTarget, TScriptInterface<IInteractable>, OldInteractionTarget);

#pragma endregion Delegates

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTRALFISHING_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

#pragma region Events

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTargetEvent OnNewInteractionTarget;

#pragma endregion Events

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "cm"))
	float TraceLength = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> Owner;

	UFUNCTION()
	bool TryInteract();

	UFUNCTION()
	bool StopInteract();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInteracting;

private:
	UPROPERTY()
	TScriptInterface<IInteractable> CurrentInteractionTarget = nullptr;

	void TraceToFindNearestInteractable();


};

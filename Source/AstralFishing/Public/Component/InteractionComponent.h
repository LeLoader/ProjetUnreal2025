

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"

#include "InteractionComponent.generated.h"

class IInteractable;
class ACharacter;
class UInputAction;
class UInputMappingContext;

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionTargetEvent, TScriptInterface<IInteractable>, NewInteractionTarget, TScriptInterface<IInteractable>, OldInteractionTarget);

#pragma endregion Delegates

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTRALFISHING_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupInputs();

protected:
	virtual void BeginPlay() override;


#pragma region Events

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTargetEvent OnNewInteractionTarget;

#pragma endregion Events

#pragma region Inputs

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InteractionMappingContext;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	/** Use Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UseAction;

#pragma endregion Inputs

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "cm"))
	float TraceLength = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> OwningCharacter;

	UFUNCTION()
	void TryInteract();

	UFUNCTION()
	bool StopInteract();

	UFUNCTION()
	void TryUse();

	UFUNCTION()
	bool StopUse();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInteracting;

private:
	UPROPERTY()
	TScriptInterface<IInteractable> CurrentInteractionTarget = nullptr;

	void TraceToFindNearestInteractable();

	uint32 InteractStartedHandle;
	uint32 UseStartedHandle;
};

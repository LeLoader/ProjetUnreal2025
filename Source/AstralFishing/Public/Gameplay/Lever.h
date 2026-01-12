

#pragma once

#include "CoreMinimal.h"
#include "Interface/Interactable.h"
#include "Gameplay/InteractableBase.h"
#include "Delegates/Delegate.h"

#include "Lever.generated.h"

class AFishermanCharacter;
class UInputMappingContext;
class UInputComponent;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ELeverDirection : uint8 {
	MIDDLE = 0,
	LEFT   = 1,
	RIGHT  = 2,
};

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLeverDirectionChangedEvent, ELeverDirection, NewDirection, ELeverDirection, OldDirection);

#pragma endregion Delegates

UCLASS(Blueprintable)
class ASTRALFISHING_API ALever : public AInteractableBase
{
	GENERATED_BODY()
	
public:	
	ALever();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

#pragma region Events

public:
	UPROPERTY(BlueprintAssignable)
	FOnLeverDirectionChangedEvent OnLeverDirectionChanged;

#pragma endregion Events

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeverBaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeverMesh;

#pragma endregion Components

#pragma region Inputs

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> LeverMappingContext;

	/** Move Lever Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveLeverAction;

	/** Move Lever Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> StopInteractAction;

#pragma endregion Inputs
	
#pragma region Interaction Implementation

public:
	FInteractionResult Interact(UInteractionComponent* InteractionSource) override;
	bool StopInteract(UInteractionComponent* InteractionSource) override;

#pragma endregion Interaction Implementation

public:
	UPROPERTY(EditAnywhere, meta = (Units = "deg", UIMin = 10, UIMax = 90, ClampMin = 10, ClampMax = 90))
	float MaxAngle = 60;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ClickSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> StopSound;

	UPROPERTY(EditAnywhere)
	float SnapSpeed = 1.f;

	UPROPERTY(EditAnywhere, meta = (Units = "Multiplier", UIMin = 0.01, UIMax = 1, ClampMin = 0.01, ClampMax = 1))
	float LeverSensitivity = 0.02f;

private:
	UPROPERTY()
	ELeverDirection CurrentDirection;

	void SetLeverDirection(ELeverDirection NewDirection);
	void MoveLever(const FInputActionValue& Value);

	float DirectionValue;
	FRotator WantedRotation;
};

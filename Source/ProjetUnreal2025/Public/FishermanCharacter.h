// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "GameFramework/Character.h"
#include "Delegates/Delegate.h"

#include "FishermanCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UInteractionComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogFishermanCharacter, Log, All);

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionTargetEvent, AActor*, NewInteractionTarget, AActor*, OldInteractionTarget);

#pragma endregion Delegates

UCLASS(config = Game)
class PROJETUNREAL2025_API AFishermanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFishermanCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

private:
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Use(const FInputActionValue& Value);
	void MoveLever(const FInputActionValue& Value);

#pragma region Events

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTargetEvent OnNewInteractionTarget;

#pragma endregion Events

#pragma region Components

private:
	// Components

#pragma endregion Components

#pragma region Inputs

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ControlsMappingContext;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	/** Use Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UseAction;

	/** Move Lever Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveLeverAction;

#pragma endregion Inputs

#pragma region Interactable

	UFUNCTION()
	void Interact(const FInputActionValue& Value);
	void TraceToFindNearestInteractable();

public:
	void StopInteract();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> CurrentInteractionTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceLength;

	bool bIsInteracting;

#pragma endregion Interactable

private:
	void AddDefaultMappingContext();
	void RemoveDefaultMappingContext();
};

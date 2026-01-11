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
 
	// Delegates
 
 #pragma endregion Delegates

UCLASS(config = Game)
class ASTRALFISHING_API AFishermanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFishermanCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

private:
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Use(const FInputActionValue& Value);

#pragma region Events

	// Events

#pragma endregion Events

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComponent;

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

#pragma endregion Inputs

#pragma region Interactable

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

#pragma endregion Interactable

private:
	void AddDefaultMappingContext();
	void RemoveDefaultMappingContext();
};



#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"


#include "Lever.generated.h"

class AFishermanCharacter;
class UInputMappingContext;
class UInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ASTRALFISHING_API ALever : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ALever();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

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
	bool Interact(AFishermanCharacter* InteractionSource) override;
	bool StopInteract(AFishermanCharacter* InteractionSource) override;

#pragma endregion Interaction Implementation

private:
	void MoveLever(const FInputActionValue& Value);
};

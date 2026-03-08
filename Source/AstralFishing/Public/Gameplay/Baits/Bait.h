

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Harpoonable.h"
#include "Enums/ReelingMethod.h"

#include "Bait.generated.h"

class UBaitDefinition;
class UProjectileMovementComponent;
class URotatingMovementComponent;

UCLASS(Blueprintable)
class ASTRALFISHING_API ABait : public AActor, public IHarpoonable
{
	GENERATED_BODY()
	
public:	
	ABait();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void PreDestroy();
	// virtual void Destroy() override;

#pragma region Harpoonable Implementation

public:
	FHarpoonResult Harpoon() override;
	bool Scan() override;

#pragma endregion Harpoonable Implementation

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BaitMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URotatingMovementComponent> RotatingComponent;

#pragma endregion Components
	
public:
	UFUNCTION(BlueprintCallable)
	void Throw(FVector Direction, float Strength);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBaitDefinition> Definition;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EReelingMethod> ReelingMethod = EReelingMethod::AUTOMATIC;

private:
	bool bThrown = false;
};

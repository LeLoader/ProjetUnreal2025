

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/AsteroidBeltElement.h"

#include "Asteroid.generated.h"

class URotatingMovementComponent;
class IAsteroidBeltElement;
class USplineComponent;

UCLASS()
class ASTRALFISHING_API AAsteroid : public AActor, public IAsteroidBeltElement
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroid();

#pragma region Components
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URotatingMovementComponent> RotatingComponent;

#pragma endregion Components

#pragma region Asteroid Belt Element Implementation

public:
	void Move(float DeltaTime, float Speed, USplineComponent* Spline) override;
	void SetCurrentDistance(float InCurrentDistance);

private:
	UPROPERTY()
	float CurrentDistance;

#pragma endregion Asteroid Belt Element Implementation
};

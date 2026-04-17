

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "AsteroidBeltElement.generated.h"

class USplineComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UAsteroidBeltElement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASTRALFISHING_API IAsteroidBeltElement
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void Move(float DeltaTime, float Speed, USplineComponent* Spline) = 0;
	
	UFUNCTION()
	virtual void SetCurrentDistance(float InCurrentDistance) = 0;
};

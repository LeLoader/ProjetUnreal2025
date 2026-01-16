

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/ReelingMethod.h"

#include "Harpoonable.generated.h"

USTRUCT(BlueprintType)
struct FHarpoonResult {

	GENERATED_BODY()

	bool bHasSuccess = false;
	EReelingMethod ReelingMethod;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UHarpoonable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASTRALFISHING_API IHarpoonable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual FHarpoonResult Harpoon() = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual bool Scan() = 0;

	
};



#pragma once

#include "ReelingMethod.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EReelingMethod : uint8
{
	NONE	  = 0 UMETA(Hidden),
	MANUAL	  = 1 UMETA(DisplayName = "Manual"),
	AUTOMATIC = 2 UMETA(DisplayName = "Automatic")
};

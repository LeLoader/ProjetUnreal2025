

#pragma once

#include "Engine/DataAsset.h"
#include "BaitDefinition.generated.h"


UCLASS(BlueprintType, Const)
class ASTRALFISHING_API UBaitDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> GenericBaitSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ThrowSound;
};

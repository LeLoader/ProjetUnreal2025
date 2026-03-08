

#pragma once

#include "CoreMinimal.h"
#include "Curves/RichCurve.h"
#include "Curves/CurveBase.h"
#include "Curves/CurveLinearColor.h"
#include "Delegates/Delegate.h"
#include "CurveAsteroidSegment.generated.h"

DECLARE_DELEGATE(FOnCurveChangedSignature);

UCLASS(BlueprintType, collapsecategories, hidecategories = (FilePath))
class ASTRALFISHING_API UCurveAsteroidSegment : public UCurveBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCurveAsteroidSegment();

public:
	UFUNCTION(BlueprintCallable, Category="Math|Curves")
	virtual float GetSizeValue(float InTime) const;

	UFUNCTION(BlueprintCallable, Category="Math|Curves")
	virtual float GetDensityValue(float InTime) const;

	UFUNCTION(BlueprintCallable, Category="Math|Curves")
	virtual float GetRadiusValue(float InTime) const;

	virtual void OnCurveChanged(const TArray<FRichCurveEditInfo>& ChangedCurveEditInfos) override;
	FOnCurveChangedSignature OnCurveHasChanged;

private:

	UPROPERTY()
	FRichCurve FloatCurves[3];

	virtual TArray<FRichCurveEditInfoConst> GetCurves() const override;
	virtual TArray<FRichCurveEditInfo> GetCurves() override;

	
};

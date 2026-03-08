


#include "CustomCurve/CurveAsteroidSegment.h"

// Sets default values for this component's properties
UCurveAsteroidSegment::UCurveAsteroidSegment()
{
#if WITH_EDITOR
	// if the curve is empty
	if (FloatCurves[0].GetNumKeys() == 0
		&& FloatCurves[1].GetNumKeys() == 0
		&& FloatCurves[2].GetNumKeys() == 0)
	{
		// Add a key for basic segment
		FloatCurves[0].AddKey(0.f, 0.5f);
		FloatCurves[1].AddKey(0.f, 0.5f);
		FloatCurves[2].AddKey(0.f, 100.f);
	}
#endif
}

float UCurveAsteroidSegment::GetSizeValue(float InTime) const
{
	return FloatCurves[0].Eval(InTime);
}

float UCurveAsteroidSegment::GetDensityValue(float InTime) const
{
	return FloatCurves[1].Eval(InTime);
}

float UCurveAsteroidSegment::GetRadiusValue(float InTime) const
{
	return FloatCurves[2].Eval(InTime);
}

void UCurveAsteroidSegment::OnCurveChanged(const TArray<FRichCurveEditInfo>& ChangedCurveEditInfos)
{
	Super::OnCurveChanged(ChangedCurveEditInfos);
	OnCurveHasChanged.ExecuteIfBound();
}

static const FName DensityCurveName(TEXT("Density"));
static const FName SizeCurveName(TEXT("Size"));
static const FName RadiusCurveName(TEXT("Radius"));

TArray<FRichCurveEditInfoConst> UCurveAsteroidSegment::GetCurves() const
{
	TArray<FRichCurveEditInfoConst> Curves;
	Curves.Add(FRichCurveEditInfoConst(&FloatCurves[0], SizeCurveName));
	Curves.Add(FRichCurveEditInfoConst(&FloatCurves[1], DensityCurveName));
	Curves.Add(FRichCurveEditInfoConst(&FloatCurves[2], RadiusCurveName));
	return Curves;
}

TArray<FRichCurveEditInfo> UCurveAsteroidSegment::GetCurves()
{
	TArray<FRichCurveEditInfo> Curves;
	Curves.Add(FRichCurveEditInfo(&FloatCurves[0], SizeCurveName));
	Curves.Add(FRichCurveEditInfo(&FloatCurves[1], DensityCurveName));
	Curves.Add(FRichCurveEditInfo(&FloatCurves[2], RadiusCurveName));
	return Curves;
}

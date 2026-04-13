


#include "CustomCurve/CurveAsteroidSegment.h"

#include <Logging/StructuredLog.h>

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

	OnUpdateCurve.AddUObject(this, &ThisClass::UpdatePrevCurves);
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

static const FName SizeCurveName(TEXT("Size"));
static const FName DensityCurveName(TEXT("Density"));
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

void UCurveAsteroidSegment::UpdatePrevCurves(UCurveBase* Curve, uint32 ChangeType)
{
	if (PrevFloatCurves[0] != FloatCurves[0]) {
		OnSizeChanged.Broadcast();
	}
	else if (PrevFloatCurves[1] != FloatCurves[1]) {
		OnDensityChanged.Broadcast();
	}
	else if (PrevFloatCurves[2] != FloatCurves[2]) {
		OnRadiusChanged.Broadcast();
	}

	PrevFloatCurves[0] = FloatCurves[0];
	PrevFloatCurves[1] = FloatCurves[1];
	PrevFloatCurves[2] = FloatCurves[2];
}

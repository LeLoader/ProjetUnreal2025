


#include "Gameplay/AsteroidBelt.h"

#include "Gameplay/Asteroid.h"
#include "Components/SplineComponent.h"
#include "CustomCurve/CurveAsteroidSegment.h"
#include <Kismet/KismetMathLibrary.h>
#include <Logging/StructuredLog.h>
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UObject/UnrealType.h"


// Sets default values
AAsteroidBelt::AAsteroidBelt()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

#if WITH_EDITOR

void AAsteroidBelt::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SegmentsCurve.IsEmpty()) {
		return;
	}

	for (UCurveAsteroidSegment* Curve : SegmentsCurve) {
		if (!IsValid(Curve)) {
			return;
		}

		if (!Curve->OnSizeChanged.IsBoundToObject(this)) {
			Curve->OnSizeChanged.AddUObject(this, &ThisClass::OnSizeChanged);
		}

		if (!Curve->OnDensityChanged.IsBoundToObject(this)) {
			Curve->OnDensityChanged.AddUObject(this, &ThisClass::OnDensityChanged);
		}

		if (!Curve->OnRadiusChanged.IsBoundToObject(this)) {
			Curve->OnRadiusChanged.AddUObject(this, &ThisClass::OnRadiusChanged);
		}
	}
}

void AAsteroidBelt::PostRegisterAllComponents() {

	Super::PostRegisterAllComponents();

	if (SegmentsCurve.IsEmpty()) {
		return;
	}

	for (UCurveAsteroidSegment* Curve : SegmentsCurve) {
		if (!IsValid(Curve)) {
			return;
		}

		if (!Curve->OnSizeChanged.IsBoundToObject(this)) {
			Curve->OnSizeChanged.AddUObject(this, &ThisClass::OnSizeChanged);
		}

		if (!Curve->OnDensityChanged.IsBoundToObject(this)) {
			Curve->OnDensityChanged.AddUObject(this, &ThisClass::OnDensityChanged);
		}

		if (!Curve->OnRadiusChanged.IsBoundToObject(this)) {
			Curve->OnRadiusChanged.AddUObject(this, &ThisClass::OnRadiusChanged);
		}
	}
}

#endif WITH_EDITOR

// Called when the game starts or when spawned
void AAsteroidBelt::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AAsteroidBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR

void AAsteroidBelt::SpawnAsteroids()
{
	if (!IsValid(SplineComponent)) {
		return;
	}

	if (SegmentsCurve.IsEmpty()) {
		return;
	}

	if (AsteroidsClasses.IsEmpty()) {
		return;
	}

	DestroyAsteroids();

	bIsCreatingAsteroids = true;

	float SegmentLength = SplineComponent->GetSplineLength() / SegmentsCurve.Num();
	for (int segmentIndex = 0; segmentIndex < SegmentsCurve.Num(); segmentIndex++)
	{
		UCurveAsteroidSegment* SegmentCurve = SegmentsCurve[segmentIndex];
		float SegmentStartDistance = SegmentLength * segmentIndex;
		float SegmentEndDistance = SegmentLength * (segmentIndex + 1);

		float CurrentDistance = SegmentStartDistance;
		float CurrentDistanceNormalized = 0;
		float const DistanceIncrement = SegmentLength / MaxAttemptPerSegment;

		// Debug
		int const MAX_ITERATION_PER_SEGMENT = 1000;
		int CurrentSegmentIteration = 0;

		while (CurrentDistance < SegmentEndDistance) {
			if (CurrentSegmentIteration > MAX_ITERATION_PER_SEGMENT) {
				UE_LOG(LogTemp, Warning, TEXT("Max iteration reached"));
				break;
			}

			int AsteroidClassIndex = FMath::RandRange(0, AsteroidsClasses.Num() - 1);

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			Params.Name = FName("Asteroid-"
				+ FString::FromInt(segmentIndex)
				+ "-");
			Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;

			FAsteroidData FutureAsteroid = FAsteroidData(AsteroidsClasses[AsteroidClassIndex], SegmentCurve, CurrentDistance, CurrentDistanceNormalized, Params);
			AsteroidCreationPool.Push(FutureAsteroid);

			CurrentDistance += DistanceIncrement;
			CurrentDistanceNormalized = CurrentDistance / SegmentEndDistance;
			CurrentSegmentIteration++;
		}
	}

	ProgressHandle = FSlateNotificationManager::Get().StartProgressNotification(FText::FromString("Spawning Asteroids..."), AsteroidCreationPool.Num());
	alreadyProcessed = 0;
	ProcessAsteroidInPool();
}

void AAsteroidBelt::DestroyAsteroids() {
	TSet<AAsteroid*> AsteroidsActors;
	Asteroids.GetKeys(AsteroidsActors);
	
	for (AAsteroid* Asteroid : AsteroidsActors)
	{
		if (IsValid(Asteroid)) {
			Asteroid->Destroy(true);
		}
	}
	Asteroids.Empty();
}

void AAsteroidBelt::CancelCurrentSpawning()
{

	bCancelRequested = true;
}

void AAsteroidBelt::ProcessAsteroidInPool()
{
	if (bCancelRequested) {
		FSlateNotificationManager::Get().UpdateProgressNotification(ProgressHandle, alreadyProcessed, 0, FText::FromString("Canceling..."));
		FSlateNotificationManager::Get().CancelProgressNotification(ProgressHandle);
		AsteroidCreationPool.Empty();
		bCancelRequested = false;
		bIsCreatingAsteroids = false;
		ProgressHandle.Reset();
		return;
	}

	if (!bIsCreatingAsteroids) {
		return;
	}

	if (AsteroidCreationPool.IsEmpty()) {
		bIsCreatingAsteroids = false;
		ProgressHandle.Reset();
		return;
	}

	float StartProcessingTime = GetWorld()->RealTimeSeconds;
	int processed = 0;
	int failed = 0;

	while (FMath::Abs(GetWorld()->RealTimeSeconds - StartProcessingTime) < MAX_PROCESSING_TIME && processed < MAX_PROCESSING_COUNT_PER_FRAME && !AsteroidCreationPool.IsEmpty()) {
		FAsteroidData AsteroidData = AsteroidCreationPool.Pop();

		if (FMath::FRand() >= AsteroidData.SegmentCurve->GetDensityValue(AsteroidData.CurrentDistanceNormalized)) {
			failed++;
			continue;
		}

		FTransform NextTransform;

		// Location
		FVector Origin = SplineComponent->GetLocationAtDistanceAlongSpline(AsteroidData.CurrentDistance, ESplineCoordinateSpace::World);
		FVector Direction = FVector(FMath::RandPointInCircle(AsteroidData.SegmentCurve->GetRadiusValue(AsteroidData.CurrentDistanceNormalized)), 0);
		float Roll = SplineComponent->GetRollAtDistanceAlongSpline(AsteroidData.CurrentDistance, ESplineCoordinateSpace::World);
		Direction.RotateAngleAxis(Roll, FVector::UpVector);
		Origin += Direction;
		NextTransform.SetLocation(Origin);

		// Rotation
		FRotator RandomRotator = UKismetMathLibrary::RandomRotator(false);
		NextTransform.SetRotation(RandomRotator.Quaternion());

		// Scale
		if (AsteroidData.SegmentCurve->GetSizeValue(AsteroidData.CurrentDistanceNormalized) < 0.1f) {
			failed++;
			continue;
		}
		NextTransform.SetScale3D(FVector::OneVector * AsteroidData.SegmentCurve->GetSizeValue(AsteroidData.CurrentDistanceNormalized));


		AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>(AsteroidData.ClassToSpawn, NextTransform, AsteroidData.SpawnParams);

		if (IsValid(Asteroid)) {

			Asteroid->SetFolderPath("Asteroids");

			Asteroids.Add(Asteroid, AsteroidData);
		}
		processed++;
	}

	alreadyProcessed += processed + failed;

	FText NewTitle = FText::FromString(FString("Creating Asteroid ") + FString::FromInt(alreadyProcessed));
	FSlateNotificationManager::Get().UpdateProgressNotification(ProgressHandle, alreadyProcessed, 0, NewTitle);
	float timeBetween = (GetWorld()->RealTimeSeconds - StartProcessingTime) * FMath::Pow(10.f, 9);
	UE_LOGFMT(LogTemp, Display, "Processed {0} spawn(s) ({1} failed) in {2} ns", processed + failed, failed, timeBetween);
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::ProcessAsteroidInPool);
}

void AAsteroidBelt::OnSizeChanged()
{
	UE_LOGFMT(LogTemp, Display, "Size Changed");

	TSet<AAsteroid*> AsteroidsActors;
	Asteroids.GetKeys(AsteroidsActors);
	

	for (AAsteroid* Asteroid : AsteroidsActors)
	{
		if (IsValid(Asteroid)) {
			FAsteroidData* AsteroidData = Asteroids.Find(Asteroid);
			Asteroid->SetActorScale3D(FVector::OneVector * AsteroidData->SegmentCurve->GetSizeValue(AsteroidData->CurrentDistanceNormalized));
		}
	}
}

void AAsteroidBelt::OnDensityChanged()
{
	UE_LOGFMT(LogTemp, Display, "Density Changed");
}

void AAsteroidBelt::OnRadiusChanged()
{
	UE_LOGFMT(LogTemp, Display, "Radius Changed");
}


void AAsteroidBelt::OnUpdateCurve(UCurveBase* Curve, uint32 ChangeType)
{
	UE_LOGFMT(LogTemp, Display, "Curve updated! {0} {1}", Curve->GetName(), ChangeType);
}

#endif WITH_EDITOR

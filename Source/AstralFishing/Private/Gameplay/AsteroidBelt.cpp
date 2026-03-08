


#include "Gameplay/AsteroidBelt.h"

#include "Gameplay/Asteroid.h"
#include "Components/SplineComponent.h"
#include "CustomCurve/CurveAsteroidSegment.h"
#include <Kismet/KismetMathLibrary.h>
#include <Logging/StructuredLog.h>

// Sets default values
AAsteroidBelt::AAsteroidBelt()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	for (UCurveAsteroidSegment* Curve : SegmentsCurve) {
		Curve->OnCurveHasChanged.BindUObject(this, &ThisClass::SpawnAsteroids);
	}
}

void AAsteroidBelt::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (SegmentsCurve.IsEmpty()){
		return;
	}

	for (UCurveAsteroidSegment* Curve : SegmentsCurve) {
		if (!IsValid(Curve)) {
			return;
		}

		Curve->OnCurveHasChanged.BindUObject(this, &ThisClass::SpawnAsteroids);
	}
}

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
		float const DistanceIncrement = SegmentLength / MaxAttempt;

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
			AsteroidCreationPool.Enqueue(FutureAsteroid);

			CurrentDistance += DistanceIncrement;
			CurrentDistanceNormalized = CurrentDistance / SegmentEndDistance;
			CurrentSegmentIteration++;
		}
	}

	ProcessAsteroidInPool();
}


void AAsteroidBelt::DestroyAsteroids() {
	for (AAsteroid* Asteroid : Asteroids)
	{
		if (IsValid(Asteroid)) {
			Asteroid->Destroy(true);
		}
	}
	Asteroids.Empty();
}

void AAsteroidBelt::ProcessAsteroidInPool()
{
	if (!bIsCreatingAsteroids) {
		return;
	}

	if (AsteroidCreationPool.IsEmpty()) {
		bIsCreatingAsteroids = false;
		return;
	}

	float StartProcessingTime = GetWorld()->RealTimeSeconds;
	int processed = 0;

	while (FMath::Abs(GetWorld()->RealTimeSeconds - StartProcessingTime) < MAX_PROCESSING_TIME && processed < MAX_PROCESSING_COUNT_PER_FRAME && !AsteroidCreationPool.IsEmpty()) {
		FAsteroidData AsteroidData;
		AsteroidCreationPool.Dequeue(AsteroidData);

		if (FMath::FRand() >= AsteroidData.SegmentCurve->GetDensityValue(AsteroidData.CurrentDistanceNormalized)) {
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
			continue;
		}
		NextTransform.SetScale3D(FVector::OneVector * AsteroidData.SegmentCurve->GetSizeValue(AsteroidData.CurrentDistanceNormalized));


		AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>(AsteroidData.ClassToSpawn, NextTransform, AsteroidData.SpawnParams);

		if (IsValid(Asteroid)) {
			Asteroid->SetFolderPath("Asteroids");
			Asteroids.Add(Asteroid);
		}
		processed++;
	}
	UE_LOGFMT(LogTemp, Display, "Processed {0} spawn(s) between {1} and {2}", processed, StartProcessingTime, GetWorld()->RealTimeSeconds);
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::ProcessAsteroidInPool);
}

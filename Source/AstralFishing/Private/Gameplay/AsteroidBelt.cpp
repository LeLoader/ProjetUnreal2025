


#include "Gameplay/AsteroidBelt.h"

#include "Gameplay/Asteroid.h"
#include "Components/SplineComponent.h"
#include "CustomCurve/CurveAsteroidSegment.h"
#include <Kismet/KismetMathLibrary.h>
#include <Logging/StructuredLog.h>
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UObject/UnrealType.h"
#include "Gameplay/Baits/Bait.h"
#include "Gameplay/Baits/BaitDefinition.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAsteroidBelt::AAsteroidBelt()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void AAsteroidBelt::BeginPlay()
{
	Super::BeginPlay();

	AsteroidBeltElements.Empty();
	TSet<AAsteroid*> AsteroidsActors;
	AsteroidsEditor.GetKeys(AsteroidsActors);
	for (AAsteroid* Asteroid : AsteroidsActors) {
		AsteroidBeltElements.Add(Asteroid);
	}
	SpawnInitialBaits();
}

void AAsteroidBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveElements(DeltaTime);
}

void AAsteroidBelt::MoveElements(float DeltaTime)
{
	for (TScriptInterface<IAsteroidBeltElement> Element : AsteroidBeltElements) {
		if (LIKELY(Element)) {
			Element->Move(DeltaTime, AsteroidsSpeed, SplineComponent);
		}
	}
}

void AAsteroidBelt::SpawnInitialBaits()
{
	if (BaitDefinitions.IsEmpty()) {
		return;
	}

	float DistanceIncrement = SplineComponent->GetSplineLength() / BaitCount;
	for (int i = 0; i < BaitCount; i++) {
		float CurrentDistance = DistanceIncrement * i;
		float CurrentDistanceNormalized = FMath::Modulo(CurrentDistance, SplineComponent->GetSplineLength());
		UCurveAsteroidSegment* CurrentCurve = GetAsteroidCurve(CurrentDistance);

		FTransform NextTransform;

		// Location
		FVector Origin = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		FVector Direction = FVector(FMath::RandPointInCircle(CurrentCurve->GetRadiusValue(CurrentDistanceNormalized)), 0);
		float Roll = SplineComponent->GetRollAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		Direction.RotateAngleAxis(Roll, FVector::UpVector);
		Origin += Direction;
		NextTransform.SetLocation(Origin);

		// Rotation
		FRotator RandomRotator = UKismetMathLibrary::RandomRotator(false);
		NextTransform.SetRotation(RandomRotator.Quaternion());

		int BaitDefinitionIndex = FMath::RandRange(0, BaitDefinitions.Num() - 1);

		ABait* Bait = GetWorld()->SpawnActorDeferred<ABait>(ABait::StaticClass(), NextTransform, this, UGameplayStatics::GetPlayerPawn(this, 0), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

		FName Label = FActorSpawnUtils::MakeUniqueActorName(GetLevel(), ABait::StaticClass(), FName(BaitDefinitions[BaitDefinitionIndex]->Name.ToString()), false);
		Bait->SetActorLabel(Label.ToString());
		Bait->SetCurrentDistance(CurrentDistance);
		Bait->Definition = BaitDefinitions[BaitDefinitionIndex];

		Bait->FinishSpawning(NextTransform);
		AsteroidBeltElements.Add(Bait);
#if WITH_EDITOR
		Bait->SetFolderPath("Baits");
#endif WITH_EDITOR
	}
}

UCurveAsteroidSegment* AAsteroidBelt::GetAsteroidCurve(float Distance)
{
	float ModulatedDistance = FMath::Modulo(Distance, SplineComponent->GetSplineLength());
	int SegmentIndex = FMath::Floor(ModulatedDistance / SplineComponent->GetSplineLength() * SegmentsCurve.Num());
	return SegmentsCurve[SegmentIndex];
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

void AAsteroidBelt::SpawnAsteroids()
{
	if (!IsValid(SplineComponent)) {
		return;
	}

	if (SegmentsCurve.IsEmpty()) {
		return;
	}

	if (AsteroidClasses.IsEmpty()) {
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

			int AsteroidClassIndex = FMath::RandRange(0, AsteroidClasses.Num() - 1);

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			Params.bNoFail = false;
			Params.Name = FName("Asteroid-"
				+ FString::FromInt(segmentIndex)
				+ "-");
			Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;

			FAsteroidData FutureAsteroid = FAsteroidData(AsteroidClasses[AsteroidClassIndex], SegmentCurve, CurrentDistance, CurrentDistanceNormalized, Params);
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
	AsteroidsEditor.GetKeys(AsteroidsActors);
	
	for (AAsteroid* Asteroid : AsteroidsActors)
	{
		if (IsValid(Asteroid)) {
			Asteroid->Destroy(true);
		}
	}
	AsteroidsEditor.Empty();
}

void AAsteroidBelt::CancelCurrentSpawning()
{

	bCancelRequested = true;
}

void AAsteroidBelt::CheckIfSegmentAreLinked()
{
	TArray<TPair<int, int>> FailedResultsIndex;
	bool bResult = AreSegmentLinked(FailedResultsIndex);
	if (!bResult) {
		UE_LOGFMT(LogTemp, Display, "Those segments are not linked: ");
		for (TPair<int, int> FailedResult : FailedResultsIndex) {
			UE_LOGFMT(LogTemp, Display, " -{0} ({1}) | {2} ({3})", SegmentsCurve[FailedResult.Key]->GetName(), FailedResult.Key, SegmentsCurve[FailedResult.Value]->GetName(), FailedResult.Value);
		}
	}
	else {
		UE_LOGFMT(LogTemp, Display, "All segment are perfectly linked!");
	}
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

	while (FMath::Abs(GetWorld()->RealTimeSeconds - StartProcessingTime) < MAX_PROCESSING_TIME && processed + failed < MAX_PROCESSING_COUNT_PER_FRAME && !AsteroidCreationPool.IsEmpty()) {
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

		UWorld* World = GetWorld();
		if (IsValid(World)) {
			FCollisionShape CollisionShape = Cast<UPrimitiveComponent>(AsteroidData.ClassToSpawn->GetDefaultObject<AActor>()->GetRootComponent())->GetCollisionShape();
			bool bBlocking = GetWorld()->OverlapBlockingTestByChannel(NextTransform.GetLocation(), NextTransform.GetRotation(), ECC_WorldDynamic, CollisionShape);
			if (bBlocking) {
				failed++;
				continue;
			}
		}


		AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>(AsteroidData.ClassToSpawn, NextTransform, AsteroidData.SpawnParams);
		Asteroid->SetCurrentDistance(AsteroidData.CurrentDistance);
		// Asteroid->GetRootComponent()->Bounds;
		// FBoxSphereBounds::BoxesIntersect(Asteroid->GetRootComponent()->Bounds, Asteroids.Get)

		if (IsValid(Asteroid)) {
			Asteroid->SetFolderPath("Asteroids");
			AsteroidsEditor.Add(Asteroid, AsteroidData);
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
	AsteroidsEditor.GetKeys(AsteroidsActors);

	for (AAsteroid* Asteroid : AsteroidsActors)
	{
		if (IsValid(Asteroid)) {
			FAsteroidData* AsteroidData = AsteroidsEditor.Find(Asteroid);
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

bool AAsteroidBelt::AreSegmentLinked(TArray<TPair<int, int>>& FailedResultsIndex) {
	bool HasFailed = false;

	for (int i = 0; i < SegmentsCurve.Num() - 2; i++)
	{
		if (!SegmentsCurve[i]->IsSegmentLinked(SegmentsCurve[i + 1])) {
			FailedResultsIndex.Add({i, i + 1});
			HasFailed = true;
		}
	}

	if (!SegmentsCurve[SegmentsCurve.Num() - 1]->IsSegmentLinked(SegmentsCurve[0])) {
		FailedResultsIndex.Add({ SegmentsCurve.Num() - 1, 0 });
		HasFailed = true;
	}

	return !HasFailed;
}

#endif WITH_EDITOR

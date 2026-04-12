

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Framework/Notifications/NotificationManager.h"

#include "UObject/UnrealType.h"

#include "AsteroidBelt.generated.h"

class USplineComponent;
class AAsteroid;
class UCurveAsteroidSegment;
class UCurveBase;

USTRUCT()
struct FAsteroidData 
{
	GENERATED_BODY()

	TSubclassOf<AAsteroid> ClassToSpawn;
	UCurveAsteroidSegment* SegmentCurve;
	float CurrentDistance;
	float CurrentDistanceNormalized;
	FActorSpawnParameters SpawnParams;

	FAsteroidData() = default;
	FAsteroidData(const TSubclassOf<AAsteroid>& ClassToSpawn, UCurveAsteroidSegment* SegmentCurve, float CurrentDistance, float CurrentDistanceNormalized, const FActorSpawnParameters& SpawnParams)
		: ClassToSpawn(ClassToSpawn), SegmentCurve(SegmentCurve), CurrentDistance(CurrentDistance), CurrentDistanceNormalized(CurrentDistanceNormalized), SpawnParams(SpawnParams) {
	}
};

UCLASS()
class ASTRALFISHING_API AAsteroidBelt : public AActor
{
	GENERATED_BODY()

public:
	AAsteroidBelt();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostRegisterAllComponents() override;
#endif WITH_EDITOR

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponent;

#pragma endregion Components

protected:
	UPROPERTY()
	TMap<AAsteroid*, FAsteroidData> Asteroids;

#if WITH_EDITOR

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt")
	void SpawnAsteroids();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt")
	void DestroyAsteroids();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt")
	void CancelCurrentSpawning();

	void OnSizeChanged();
	void OnDensityChanged();
	void OnRadiusChanged();

#endif WITH_EDITOR

private:
	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	TArray<TSubclassOf<AAsteroid>> AsteroidsClasses;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	TArray<UCurveAsteroidSegment*> SegmentsCurve;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	int MaxAttemptPerSegment = 100;

	TArray<struct FAsteroidData> AsteroidCreationPool;
	bool bIsCreatingAsteroids;
	void ProcessAsteroidInPool();

	UPROPERTY(EditAnywhere, Category = "Editor|Asteroid Belt")
	float MAX_PROCESSING_TIME = 1.f / 60.f;

	UPROPERTY(EditAnywhere, Category = "Editor|Asteroid Belt")
	int MAX_PROCESSING_COUNT_PER_FRAME = 10;

	FProgressNotificationHandle ProgressHandle;
	int alreadyProcessed = -1;

	bool bCancelRequested = false;

	// should be EPropertyChangeType::Type instead of uint32 but can seem to make it recognized by the compiler
	UFUNCTION()
	void OnUpdateCurve(UCurveBase* Curve, uint32 ChangeType);
};



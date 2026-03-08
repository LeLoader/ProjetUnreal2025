

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidBelt.generated.h"

class USplineComponent;
class AAsteroid;
class UCurveAsteroidSegment;

UCLASS()
class ASTRALFISHING_API AAsteroidBelt : public AActor
{
	GENERATED_BODY()

public:
	AAsteroidBelt();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponent;

#pragma endregion Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asteroid Belt")
	TArray<AAsteroid*> Asteroids;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt")
	void SpawnAsteroids();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt")
	void DestroyAsteroids();

private:
	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	TArray<TSubclassOf<AAsteroid>> AsteroidsClasses;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	TArray<UCurveAsteroidSegment*> SegmentsCurve;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt")
	int MaxAttempt = 100;

	TQueue<struct FAsteroidData> AsteroidCreationPool;
	bool bIsCreatingAsteroids;
	void ProcessAsteroidInPool();

	UPROPERTY(EditAnywhere, Category = "Editor|Asteroid Belt")
	float MAX_PROCESSING_TIME = 1.f / 60.f;

	UPROPERTY(EditAnywhere, Category = "Editor|Asteroid Belt")
	int MAX_PROCESSING_COUNT_PER_FRAME = 10;
};	

struct FAsteroidData {
	TSubclassOf<AAsteroid> ClassToSpawn;
	UCurveAsteroidSegment* SegmentCurve;
	float CurrentDistance;
	float CurrentDistanceNormalized;
	FActorSpawnParameters SpawnParams;

	FAsteroidData() = default;
	FAsteroidData(const TSubclassOf<AAsteroid>& ClassToSpawn, UCurveAsteroidSegment* SegmentCurve, float CurrentDistance, float CurrentDistanceNormalized, const FActorSpawnParameters& SpawnParams)
		: ClassToSpawn(ClassToSpawn), SegmentCurve(SegmentCurve), CurrentDistance(CurrentDistance), CurrentDistanceNormalized(CurrentDistanceNormalized), SpawnParams(SpawnParams) { }
};

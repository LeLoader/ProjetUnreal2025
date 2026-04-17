

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Framework/Notifications/NotificationManager.h"

#include "UObject/UnrealType.h"

#include "AsteroidBelt.generated.h"

class USplineComponent;
class UCurveAsteroidSegment;
class UCurveBase;
class AAsteroid;
class ABait;
class IAsteroidBeltElement;

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

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponent;

#pragma endregion Components

protected:
	UPROPERTY()
	TMap<AAsteroid*, FAsteroidData> AsteroidsEditor;

	UPROPERTY(VisibleAnywhere)
	TArray<TScriptInterface<IAsteroidBeltElement>> AsteroidBeltElements;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt", meta = (Units = "cm/s"))
	float AsteroidsSpeed = 100;

	UFUNCTION()
	void MoveElements(float DeltaTime);

	UFUNCTION()
	UCurveAsteroidSegment* GetAsteroidCurve(float Distance);

#pragma region Baits

protected:
	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Baits")
	TArray<class UBaitDefinition*> BaitDefinitions;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Baits")
	int BaitCount = 20;

	UFUNCTION()
	void SpawnInitialBaits();

#pragma endregion Baits

#pragma region Editor

#if WITH_EDITOR

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostRegisterAllComponents() override;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt|Editor")
	void SpawnAsteroids();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt|Editor")
	void DestroyAsteroids();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt|Editor")
	void CancelCurrentSpawning();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Asteroid Belt|Editor")
	void CheckIfSegmentAreLinked();

	void OnSizeChanged();
	void OnDensityChanged();
	void OnRadiusChanged();
	bool AreSegmentLinked(TArray<TPair<int, int>>& FailedResultsIndex);

#endif WITH_EDITOR

private:
	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Editor")
	TArray<TSubclassOf<AAsteroid>> AsteroidClasses;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Editor")
	TArray<UCurveAsteroidSegment*> SegmentsCurve;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Editor")
	int MaxAttemptPerSegment = 100;

	TArray<struct FAsteroidData> AsteroidCreationPool;
	bool bIsCreatingAsteroids;
	void ProcessAsteroidInPool();

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Editor")
	float MAX_PROCESSING_TIME = 1.f / 60.f;

	UPROPERTY(EditAnywhere, Category = "Asteroid Belt|Editor")
	int MAX_PROCESSING_COUNT_PER_FRAME = 10;

	FProgressNotificationHandle ProgressHandle;
	int alreadyProcessed = -1;

	bool bCancelRequested = false;

#pragma endregion Editor
};



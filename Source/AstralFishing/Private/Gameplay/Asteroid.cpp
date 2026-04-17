


#include "Gameplay/Asteroid.h"

#include "GameFramework/RotatingMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/SplineComponent.h"

AAsteroid::AAsteroid()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Asteroid"));
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	RootComponent = MeshComponent;

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	FRotator RandomRotator;
	RandomRotator.Yaw = FMath::FRand();
	RandomRotator.Pitch = FMath::FRand();
	RotatingComponent->RotationRate = RandomRotator;
}

void AAsteroid::Move(float DeltaTime, float Speed, USplineComponent* Spline)
{
	CurrentDistance = FMath::Modulo(CurrentDistance + Speed * DeltaTime, Spline->GetSplineLength());
	FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	RootComponent->SetRelativeLocation_Direct(NewLocation);
	RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);
}

void AAsteroid::SetCurrentDistance(float InCurrentDistance)
{
	CurrentDistance = InCurrentDistance;
}

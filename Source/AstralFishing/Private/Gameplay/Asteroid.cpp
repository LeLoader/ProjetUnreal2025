


#include "Gameplay/Asteroid.h"

#include "GameFramework/RotatingMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>


// Sets default values
AAsteroid::AAsteroid()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Asteroid"));
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	RootComponent = MeshComponent;

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	FRotator RandomRotator = UKismetMathLibrary::RandomRotator(false);
	FRotator ScaledDownRandomRotator = FRotator(RandomRotator.Pitch / 180, RandomRotator.Yaw / 180, RandomRotator.Roll / 180);
	RotatingComponent->RotationRate = ScaledDownRandomRotator;

}



#include "Gameplay/Asteroid.h"

#include "GameFramework/RotatingMovementComponent.h"


// Sets default values
AAsteroid::AAsteroid()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Asteroid"));
	RootComponent = MeshComponent;

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
}
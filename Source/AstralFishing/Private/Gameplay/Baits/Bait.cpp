


#include "Gameplay/Baits/Bait.h"

#include "Gameplay/Baits/BaitDefinition.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Components/SplineComponent.h"

#define ECC_Bait ECC_GameTraceChannel4

// Sets default values
ABait::ABait()
{
	PrimaryActorTick.bCanEverTick = false;

	BaitMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Bait"));
	BaitMeshComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BaitMeshComponent->SetCollisionObjectType(ECC_Bait);
	RootComponent = BaitMeshComponent;

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->SetVelocityInLocalSpace(FVector::Zero());
	ProjectileComponent->ProjectileGravityScale = 0;
	ProjectileComponent->Deactivate();
	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	FRotator RandomRotator;
	RandomRotator.Yaw = FMath::FRand();
	RandomRotator.Pitch = FMath::FRand();
	RotatingComponent->RotationRate = RandomRotator;
}

void ABait::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (IsValid(Definition)) {
		BaitMeshComponent->SetStaticMesh(Definition->StaticMesh);
	}
}

void ABait::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Definition)) {
		BaitMeshComponent->SetStaticMesh(Definition->StaticMesh);
	}
}

void ABait::Throw(FVector Direction, float Strength)
{
	bThrown = true;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Definition->ThrowSound, GetActorLocation());

	// Destroy timer (in case sent in space)
	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::PreDestroy, 60.0f, false);

	// Random rotation
	FRotator RandomRotator;
	RandomRotator.Yaw = FMath::FRand() * 360.f;
	RandomRotator.Pitch = FMath::FRand() * 360.f;
	RotatingComponent->RotationRate = RandomRotator;

	// Set velocity
	Direction.Normalize();
	ProjectileComponent->SetVelocityInLocalSpace(Direction * Strength);
	ProjectileComponent->Activate();
}

void ABait::Move(float DeltaTime, float Speed, USplineComponent* Spline)
{
	if (bThrown) {
		UE_LOGFMT(LogTemp, Warning, "Trying to move a bait which has been thrown");
		return;
	}

	CurrentDistance = FMath::Modulo(CurrentDistance + Speed * DeltaTime, Spline->GetSplineLength());
	FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	RootComponent->SetRelativeLocation_Direct(NewLocation);
	RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);
}

void ABait::SetCurrentDistance(float InCurrentDistance)
{
	CurrentDistance = InCurrentDistance;
}

FHarpoonResult ABait::Harpoon()
{
	// Add harpoon as param
	// When harpooned, the bait should attach to the harpoon
	RotatingComponent->Deactivate();

	return FHarpoonResult(true, ReelingMethod);
}

bool ABait::Scan()
{
	// Add scanner as param
	// When scanned, the bait should be outlined a bit like the other stuff, but different from interaction outline

	throw std::logic_error("The method or operation is not implemented.");
}

void ABait::PreDestroy()
{
	Destroy();
}

// void ABait::Destroy()
// {
// 	Super::Destroy();
// 
// 	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Definition->GenericBaitSound, GetActorLocation());
// }

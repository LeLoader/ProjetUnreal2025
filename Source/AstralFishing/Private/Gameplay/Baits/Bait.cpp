


#include "Gameplay/Baits/Bait.h"

#include "Gameplay/Baits/BaitDefinition.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABait::ABait()
{
	PrimaryActorTick.bCanEverTick = false;

	BaitMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM Bait"));
	RootComponent = BaitMeshComponent;

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
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
}

FHarpoonResult ABait::Harpoon()
{
	// Add harpoon as param
	// When harpooned, the bait should attach to the harpoon

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

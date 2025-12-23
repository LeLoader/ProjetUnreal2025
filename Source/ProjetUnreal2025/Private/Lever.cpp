


#include "Lever.h"


// Sets default values
ALever::ALever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALever::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALever::Interact(AFishermanCharacter* Instigator)
{
	throw std::logic_error("The method or operation is not implemented.");
}

int ALever::GetPriority()
{
	throw std::logic_error("The method or operation is not implemented.");
}


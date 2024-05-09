// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHGlobalResourceManager.h"

#include "Components/BoxComponent.h"
#include "Game/RHGameStateBase.h"
#include "Resources/RHResourceBase.h"


// Sets default values
ARHGlobalResourceManager::ARHGlobalResourceManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the BoxComponent
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;
	SpawnVolume->SetBoxExtent(FVector(500, 500, 100)); // Default size, adjust as needed
	SpawnVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Make sure it doesn't physically interact

}

// Called when the game starts or when spawned
void ARHGlobalResourceManager::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority()) //only spawn resources in the server
	{
		// Get the game state and bind to the delegate
		ARHGameStateBase* GameState = GetWorld()->GetGameState<ARHGameStateBase>();
		if (GameState)
		{
			GameState->OnAllPlayersReady.AddDynamic(this, &ARHGlobalResourceManager::OnAllPlayersReady);
		}
	}
}

void ARHGlobalResourceManager::SpawnResource1()
{
	int32 ResourcesToSpawn = 6;  // Number of resources to spawn each time function is called

	// Only spawn if it does not exceed the maximum count
	if (SpawnedResources1.Num() + ResourcesToSpawn <= MaxResource1Count)
	{
		for (int32 i = 0; i < ResourcesToSpawn; i++)
		{
			FVector SpawnLocation = GenerateRandomPointInBox();
			SpawnResourceAtLocation(ResourceClassOrdinary, SpawnLocation);
		}
	}
}

void ARHGlobalResourceManager::SpawnResource2() const
{
	if (constexpr int32 ResourcesToSpawn = 3; SpawnedResources2.Num() + ResourcesToSpawn < MaxResource2Count)
	{
		for (int32 i = 0; i < ResourcesToSpawn; i++)
		{
			FVector SpawnLocation = GenerateRandomPointInBox();
			SpawnResourceAtLocation(ResourceClassEpic, SpawnLocation);
		}
	}
}

FVector ARHGlobalResourceManager::GenerateRandomPointInBox() const
{
	const FVector Origin = SpawnVolume->GetComponentLocation();
	const FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

	const FVector RandomPoint = Origin + FVector(
		FMath::RandRange(-BoxExtent.X, BoxExtent.X),
		FMath::RandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::RandRange(-BoxExtent.Z, BoxExtent.Z)
	);

	return RandomPoint;
}

void ARHGlobalResourceManager::SpawnResourceAtLocation(const TArray<TSubclassOf<ARHResourceBase>>& Resources, const FVector& Location) const
{
	const TSubclassOf<ARHResourceBase> ChosenResource = Resources[FMath::RandRange(0, Resources.Num() - 1)];
	if (AActor* Spawned = GetWorld()->SpawnActor<ARHResourceBase>(ChosenResource, Location, FRotator::ZeroRotator))
	{
		Spawned->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

//alias, on game starts!!
void ARHGlobalResourceManager::OnAllPlayersReady()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Resource1, this, &ARHGlobalResourceManager::SpawnResource1, 18.0f, true, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_Resource2, this, &ARHGlobalResourceManager::SpawnResource2, 20.0f, true, 60.0f);
}

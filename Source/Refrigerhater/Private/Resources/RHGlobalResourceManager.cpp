// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHGlobalResourceManager.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	GetWorldTimerManager().SetTimer(TimerHandle_Resource1, this, &ARHGlobalResourceManager::SpawnResource1, 30.0f, true, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_Resource2, this, &ARHGlobalResourceManager::SpawnResource2, 30.0f, true, 120.0f);
	
}

void ARHGlobalResourceManager::SpawnResource1()
{
	if (SpawnedResources1.Num() < MaxResource1Count)
	{
		FVector SpawnLocation = GenerateRandomPointInBox();
		SpawnResourceAtLocation(ResourceClassOrdinary, SpawnLocation);
	}
}

void ARHGlobalResourceManager::SpawnResource2()
{
	if (SpawnedResources2.Num() < MaxResource2Count)
	{
		FVector SpawnLocation = GenerateRandomPointInBox();
		SpawnResourceAtLocation(ResourceClassEpic, SpawnLocation);
	}
}

FVector ARHGlobalResourceManager::GenerateRandomPointInBox()
{
	FVector Origin = SpawnVolume->GetComponentLocation();
	FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

	FVector RandomPoint = Origin + FVector(
		FMath::RandRange(-BoxExtent.X, BoxExtent.X),
		FMath::RandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::RandRange(-BoxExtent.Z, BoxExtent.Z)
	);

	return RandomPoint;
}

void ARHGlobalResourceManager::SpawnResourceAtLocation(const TArray<TSubclassOf<ARHResourceBase>>& Resources, const FVector& Location)
{
	TSubclassOf<ARHResourceBase> ChosenResource = Resources[FMath::RandRange(0, Resources.Num() - 1)];
	AActor* Spawned = GetWorld()->SpawnActor<ARHResourceBase>(ChosenResource, Location, FRotator::ZeroRotator);
	if (Spawned)
	{
		Spawned->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	}
}
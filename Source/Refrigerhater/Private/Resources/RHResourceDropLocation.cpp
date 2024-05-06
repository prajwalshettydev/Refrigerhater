// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHResourceDropLocation.h"

#include "RHGameModeBase.h"
#include "Components/BoxComponent.h"


// Sets default values
ARHResourceDropLocation::ARHResourceDropLocation()
{
	PrimaryActorTick.bCanEverTick = false;

	DropZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DropZone"));
	DropZone->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = DropZone;

	DropZone->OnComponentBeginOverlap.AddDynamic(this, &ARHResourceDropLocation::OnResourceDropped);
}

void ARHResourceDropLocation::BeginPlay()
{
	Super::BeginPlay();
}

void ARHResourceDropLocation::OnResourceDropped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// This function would be triggered when a pawn enters the drop zone.
	// Actual inventory checking and resource dropping logic should be handled here.
	// Example:
	// NotifyDropReceived("ResourceTypeExample", 1);
}

void ARHResourceDropLocation::NotifyDropReceived(const FString& ResourceType, int32 Quantity)
{
	// Here you would contact the GameMode to update points or similar
	// This must run on the server where the GameMode exists
	if (HasAuthority())
	{
		// Cast to your custom GameMode and call a function to handle the score update
		ARHGameModeBase* GM = Cast<ARHGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			//GM->AddPointsForTeam(ResourceType, Quantity, GetTeamID());  // Implement this function in your GameMode
		}
	}
}
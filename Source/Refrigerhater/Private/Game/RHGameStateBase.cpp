// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RHGameStateBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARHGameStateBase::ARHGameStateBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void ARHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARHGameStateBase, PlayerReadiness);
	DOREPLIFETIME(ARHGameStateBase, bArePlayersReady);
	DOREPLIFETIME(ARHGameStateBase, GameTimeSeconds);
}

FString ARHGameStateBase::GetFormattedGameTime() const
{
	const int32 TotalSeconds = FMath::FloorToInt(GameTimeSeconds);
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	// Format time as MM:SS
	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void ARHGameStateBase::OnRep_PlayersReady() const
{
	OnAllPlayersReady.Broadcast();
}

void ARHGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    
	if (HasAuthority())
	{
		// Check if the game has started
		if (bArePlayersReady)
		{
			if (GameTimeSeconds > 0)  // Make sure the time doesn't go below zero
			{
				TimeSinceLastReplication += DeltaSeconds;
				if (TimeSinceLastReplication >= ReplicationInterval)
				{
					// Reduce the GameTimeSeconds by the time passed since last update
					GameTimeSeconds -= TimeSinceLastReplication;
					TimeSinceLastReplication = 0.0f;

					// Ensure the game time does not drop below zero
					if (GameTimeSeconds < 0)
					{
						GameTimeSeconds = 0;
					}
				}
			}
		}
		else
		{
			// Reset the timer if the players are not ready
			GameTimeSeconds = 240.0f; // Set to total round time (4 minutes)
		}
	}
}

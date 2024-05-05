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
		if(!bArePlayersReady)
		{
			GameTimeSeconds = 0;
		}
		else {
			TimeSinceLastReplication += DeltaSeconds;
			if (TimeSinceLastReplication >= ReplicationInterval)
			{
				GameTimeSeconds += TimeSinceLastReplication;
				TimeSinceLastReplication = 0.0f; // Reset the timer after updating
			}
		}
	}
}

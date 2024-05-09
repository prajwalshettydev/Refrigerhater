// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RHGameStateBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARHGameStateBase::ARHGameStateBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//any new resources we add can be easily be appended here, or in the future if we have 10 to 50+ resource then this can also moved to a dynamic mapping function or the child resource objects can tag themselves as particular resources.
	ResourceCategories.Add(EResourceType::BigWatermelon, EResourceCategory::Legendary);
	ResourceCategories.Add(EResourceType::BeerCrate, EResourceCategory::Ordinary);
	ResourceCategories.Add(EResourceType::TomatoCrate, EResourceCategory::Ordinary);
	ResourceCategories.Add(EResourceType::BananaCrate, EResourceCategory::Ordinary);
}


void ARHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARHGameStateBase, PlayerReadiness);
	DOREPLIFETIME(ARHGameStateBase, bArePlayersReady);
	DOREPLIFETIME(ARHGameStateBase, GameTimeSeconds);
	DOREPLIFETIME(ARHGameStateBase, TeamAScore);
	DOREPLIFETIME(ARHGameStateBase, TeamBScore);
}

// GameState.cpp
void ARHGameStateBase::AddScore(EResourceType ResourceType, bool IsTeamA)
{
	const int32 Points = (ResourceCategories[ResourceType] == EResourceCategory::Legendary) ? 8 : 4;

	if (IsTeamA)
	{
		TeamAScore += Points;
	}
	else
	{
		TeamBScore += Points;
	}
}


FString ARHGameStateBase::GetFormattedGameTime() const
{
	const int32 TotalSeconds = FMath::FloorToInt(GameTimeSeconds);
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	// Format time as MM:SS
	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void ARHGameStateBase::MulticastAnnounceWinner_Implementation(const bool bIsTeamA)
{
	OnGameAnnouncesWinner.Broadcast(bIsTeamA);
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
					if (GameTimeSeconds <= 0)
					{
						GameTimeSeconds = 0;
						DeclareWinnerAndCleanup();
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

void ARHGameStateBase::DeclareWinnerAndCleanup()
{
	// Determine the winning team
	const bool bIsTeamAWinner = TeamAScore > TeamBScore;
	//FString winner = isTeamAWinner ? TEXT("Team A Wins!") : TEXT("Team B Wins!");
	
	// Broadcast to all clients
	MulticastAnnounceWinner(bIsTeamAWinner);

	// Server side cleanup for next match
	TeamAScore = 0;
	TeamBScore = 0;
	bArePlayersReady = false;
}

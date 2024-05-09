// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\Player\RHBasePlayerController.h"

#include "RHCustomLog.h"
#include "RHGameModeBase.h"
#include "Player/RHBasePlayer.h"

void ARHBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}


ARHBasePlayerController::ARHBasePlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}


// In ARHPlayerController.cpp
void ARHBasePlayerController::ServerPlayerReady_Implementation(int32 SelectedTeam, const EFridgeType FridgeType)
{
	ARHGameModeBase* GM = Cast<ARHGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->OnSpecificPlayerIsReady(this, SelectedTeam, FridgeType);
	}
}

bool ARHBasePlayerController::ServerPlayerReady_Validate(int32 SelectedTeam, const EFridgeType FridgeType)
{
	// Add validation logic if needed
	return true;  // Simplified for example
}

void ARHBasePlayerController::CallServerPlayerReady(int32 SelectedTeam, const EFridgeType FridgeType)
{
	ServerPlayerReady(SelectedTeam, FridgeType);
}


/**
 * Takes string inputs for team name and fridge types,
 * can be use alternative to CallServerPlayerReady, which uses int and enums
 * NOTE: for invalid string inputs, Team B and Single Door are picked as defualts
 * @param TeamName 
 * @param FridgeTypeName 
 */
void ARHBasePlayerController::PrepareAndCallServerPlayerReady(const FString& TeamName, const FString& FridgeTypeName)
{
	// Convert TeamName to team index
	const int32 TeamIndex = (TeamName == "Team A") ? 0 : 1; // Assuming only two teams: Team A and Team B

	// Convert FridgeTypeName to EFridgeType
	EFridgeType FridgeType = EFridgeType::SingleDoor; // Default to SingleDoor
	if (FridgeTypeName == "Double Door")
	{
		FridgeType = EFridgeType::DoubleDoor;
	}
	else if (FridgeTypeName == "Mini Fridge")
	{
		FridgeType = EFridgeType::MiniFridge;
	}

	// Call the existing function with converted values
	CallServerPlayerReady(TeamIndex, FridgeType);
}

void ARHBasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

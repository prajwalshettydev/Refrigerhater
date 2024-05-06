// Fill out your copyright notice in the Description page of Project Settings.


#include "RHGameModeBase.h"

#include "EngineUtils.h"
#include "RHCustomLog.h"
#include "Game/RHGameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RHBasePlayer.h"
#include "Player/RHBasePlayerController.h"
#include "Player/RHBasePlayerState.h"


ARHGameModeBase::ARHGameModeBase()
{
	//Setup all the default classes for the game mode
	PlayerControllerClass = ARHBasePlayerController::StaticClass();
	DefaultPawnClass = ARHBasePlayer::StaticClass();
	PlayerStateClass = ARHBasePlayerState::StaticClass();
	GameStateClass = ARHGameStateBase::StaticClass();

	bStartPlayersAsSpectators = true;
	
	CacheTeamSpawnPoints();
}


/**
 * Cache team spawn points just once during construction
 */
void ARHGameModeBase::CacheTeamSpawnPoints()
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);

	Team1SpawnPoints.Empty();
	Team2SpawnPoints.Empty();
	
	for (AActor* SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint->Tags.Contains("Team1"))
		{
			Team1SpawnPoints.Add(SpawnPoint);
		}
		else if (SpawnPoint->Tags.Contains("Team2"))
		{
			Team2SpawnPoints.Add(SpawnPoint);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("Team 1 Spawn Points: %d, Team 2 Spawn Points: %d"), Team1SpawnPoints.Num(), Team2SpawnPoints.Num());
}


/**
 * This is called every time a new player joins the game, it's also called 'Event OnPostLogin' in blueprints
 * @param NewPlayer 
 */
void ARHGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Make sure the cast was successful and the pawn is valid
	if (const ARHBasePlayer* NewPlayerPawn = Cast<ARHBasePlayer>(NewPlayer->GetPawn()))
	{
		UE_LOG(LogPlayer, Log, TEXT("Player Connected %s"), *NewPlayerPawn->PlayerName);
		
		// Now call the function on the pawn that updates all clients with the initial state
		//NewPlayerPawn->UpdateAllClientsWithInitialState();
	}
}

/**
 * Our custom game start function not to be confused with unreal's own overridable start game related functions in game mode base
 */
void ARHGameModeBase::StartGameCustom()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AController* PlayerController = It->Get();
		if (PlayerController && !PlayerController->GetPawn()) 
		{
			// we are manually spawning the players here based on the teams they have selected
			SpawnPlayerAtTeamStart(PlayerController); 
		}
	}
	
	if (ARHGameStateBase* MyGameState = GetGameState<ARHGameStateBase>())
	{
		UE_LOG(LogPlayer, Log, TEXT("Everyones ready lets go"));
		MyGameState->bArePlayersReady = true; // This will trigger OnRep_PlayersReady on all clients
	}
}


/**
 * A particular player has informed the game mode base in the server that it is ready to play!
 * @param PlayerController 
 * @param PlayerTeam 
 * @param FridgeType 
 */
void ARHGameModeBase::OnSpecificPlayerIsReady(const APlayerController* PlayerController, const int32 PlayerTeam, const EFridgeType FridgeType)
{
	UE_LOG(LogPlayer, Log, TEXT("Player ready %s"), *PlayerController->GetName());
	if (!IsTeamAndFridgeValid(PlayerTeam, FridgeType))
	{
		// Optionally send a message back to the player about invalid selection
		return;
	}
	
	ARHBasePlayerState* PlayerState = PlayerController->GetPlayerState<ARHBasePlayerState>();
	PlayerState->Team = PlayerTeam;
	PlayerState->SelectedFridgeType = FridgeType;
	PlayerState->Ready = true;
	

	PlayerTeamAssignments.Add(PlayerController->PlayerState, PlayerTeam);
	PlayerFridgeSelections.Add(PlayerController->PlayerState, FridgeType);

	if (AreAllPlayersReady())
	{
		StartGameCustom();
	}
}

void ARHGameModeBase::AddPointsForTeam(const FString& ResourceType, int32 Quantity, int32 TeamID)
{
	// Implement logic to add points based on the resource type and quantity
	// Example: update a team's score
}

#pragma region helpers 

bool ARHGameModeBase::AreAllPlayersReady()
{
	const int32 NumConnectedPlayers = GetNumPlayers(); // Get the total number of players connected
	int32 ReadyPlayers = 0;

	// Iterate over all player states to check if each player is ready
	for (APlayerState* PlayerState : TActorRange<APlayerState>(GetWorld()))
	{
		const ARHBasePlayerState* RHPlayerState = Cast<ARHBasePlayerState>(PlayerState);
		if (RHPlayerState && RHPlayerState->Ready)
		{
			ReadyPlayers++;
		}
	}

	UE_LOG(LogPlayer, Log, TEXT("Total players connected: %d, Ready Players: %d"), NumConnectedPlayers, ReadyPlayers);

	// Check if the number of ready players equals the number of connected players
	return ReadyPlayers == NumConnectedPlayers;
}

bool ARHGameModeBase::IsTeamAndFridgeValid(int32 PlayerTeam, const EFridgeType FridgeType)
{
	// Implement checks for team limits and fridge type validity
	return true; // Simplified for example
}

void ARHGameModeBase::SpawnPlayerAtTeamStart(AController* Controller)
{
	if (AActor* StartSpot = ChoosePlayerStart(Controller))
	{
		RestartPlayerAtPlayerStart(Controller, StartSpot);
	}
}

	
/**
 * Since ChoosePlayerStart is defined with UFUNCTION(BlueprintNativeEvent, Category=Game),
 * this means we need to use the _Implementation suffix in your override implementation.
 * @param Player 
 * @return 
 */
AActor* ARHGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	ARHBasePlayerState* PlayerState = Player->GetPlayerState<ARHBasePlayerState>();
	if (PlayerState)
	{
		TArray<AActor*>* TeamSpawnPoints = (PlayerState->Team == 0) ? &Team1SpawnPoints : &Team2SpawnPoints;

		if (TeamSpawnPoints->Num() > 0)
		{
			int32 Index = FMath::RandRange(0, TeamSpawnPoints->Num() - 1);
			return (*TeamSpawnPoints)[Index];
		}
	}
	//return nullptr; // Fallback if no spawn points found

	// Fallback to default logic
	return Super::ChoosePlayerStart_Implementation(Player);
}


bool ARHGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;  // Prevent auto-spawn
}


void ARHGameModeBase::RestartPlayerAtPlayerStart(AController* Controller, AActor* StartSpot)
{
	if (Controller)
	{
		ARHBasePlayerState* PlayerState = Controller->GetPlayerState<ARHBasePlayerState>();
		if (PlayerState)
		{
			UClass* PlayerClass = GetPlayerClassForFridgeType(PlayerState->SelectedFridgeType);
			if (PlayerClass)
			{
				APawn* Pawn = GetWorld()->SpawnActor<APawn>(PlayerClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
				Controller->Possess(Pawn);
				return;
			}
		}
	}

	// Fallback if no custom logic is needed or valid pawn class is not found
	Super::RestartPlayerAtPlayerStart(Controller, StartSpot);
}

UClass* ARHGameModeBase::GetPlayerClassForFridgeType(EFridgeType FridgeType) const
{
	switch (FridgeType)
	{
		case EFridgeType::SingleDoor:
			return SingleDoorFridgePlayerClass;
		case EFridgeType::DoubleDoor:
			return DoubleDoorFridgePlayerClass;
		case EFridgeType::MiniFridge:
			return MiniFridgePlayerClass;
		default:
			return ARHBasePlayer::StaticClass();  // Fallback player class
	}
}

#pragma endregion
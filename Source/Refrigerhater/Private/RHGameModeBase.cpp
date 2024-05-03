// Fill out your copyright notice in the Description page of Project Settings.


#include "RHGameModeBase.h"

#include "RHCustomLog.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RHBasePlayer.h"
#include "Player/RHBasePlayerController.h"
#include "Player/RHBasePlayerState.h"


ARHGameModeBase::ARHGameModeBase()
{
	PlayerControllerClass = ARHBasePlayerController::StaticClass();
	DefaultPawnClass = ARHBasePlayer::StaticClass();
	PlayerStateClass = ARHBasePlayerState::StaticClass();

	bStartPlayersAsSpectators = true;
	
	CacheTeamSpawnPoints();
	
	// // Blueprinted Version, relies on the asset path obtained from the editor
	// static ConstructorHelpers::FClassFinder<ARHBasePlayer> PlayerPawnClassFinder(TEXT("/Game/Player/BP_SingleDoorFridge.BP_SingleDoorFridge"));
	// if (PlayerPawnClassFinder.Succeeded())
	// {
	// 	DefaultPawnClass = PlayerPawnClassFinder.Class;
	// }
	// else
	// {
	// 	UE_LOG(LogPlayer, Error, TEXT("Failed to find the player pawn bp class"));
	// }
}


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

void ARHGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ARHGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void ARHGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Cast the NewPlayer's controlled pawn to your player class, if that's where your data is stored
	ARHBasePlayer* NewPlayerPawn = Cast<ARHBasePlayer>(NewPlayer->GetPawn());

	// Make sure the cast was successful and the pawn is valid
	if (NewPlayerPawn)
	{
		UE_LOG(LogPlayer, Log, TEXT("Player Connected %s"), *NewPlayerPawn->PlayerName);
		// Now call the function on the pawn that updates all clients with the initial state
		//NewPlayerPawn->UpdateAllClientsWithInitialState();
	}
	else
	{
		UE_LOG(LogPlayer, Error, TEXT("Failed to find the player pawn"));
	}
}

void ARHGameModeBase::StartGame()
{
	UE_LOG(LogPlayer, Error, TEXT("Starting game"));
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AController* PlayerController = It->Get();
		if (PlayerController && !PlayerController->GetPawn()) // Check if the player has no pawn yet
		{
			SpawnPlayerAtTeamStart(PlayerController); // Custom function to handle spawning logic based on teams or any other criteria
		}
	}
}
void ARHGameModeBase::ServerPlayerReady_Implementation(APlayerController* PlayerController, int32 PlayerTeam, const FString& FridgeType)
{
	if (!IsTeamAndFridgeValid(PlayerTeam, FridgeType))
	{
		// Optionally send a message back to the player about invalid selection
		return;
	}

	PlayerTeamAssignments.Add(PlayerController->PlayerState, PlayerTeam);
	PlayerFridgeSelections.Add(PlayerController->PlayerState, FridgeType);

	if (AreAllPlayersReady())
	{
		StartGame();
	}
}

bool ARHGameModeBase::ServerPlayerReady_Validate(APlayerController* PlayerController, int32 PlayerTeam, const FString& FridgeType)
{
	// Add validation logic here if necessary
	return true;
}

bool ARHGameModeBase::AreAllPlayersReady()
{
	// Implement logic to check if all players are ready
	// This might include checking the number of players and their readiness state
	int32 NumConnectedPlayers = GetNumPlayers(); // Get the total number of players connected
	return PlayerTeamAssignments.Num() == NumConnectedPlayers && PlayerFridgeSelections.Num() == NumConnectedPlayers;

}

bool ARHGameModeBase::IsTeamAndFridgeValid(int32 PlayerTeam, const FString& FridgeType) const
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

AActor* ARHGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	ARHBasePlayerState* PlayerState = Player->GetPlayerState<ARHBasePlayerState>();
	if (PlayerState)
	{
		TArray<AActor*>* TeamSpawnPoints = (PlayerState->Team == 1) ? &Team1SpawnPoints : &Team2SpawnPoints;

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

UClass* ARHGameModeBase::GetPlayerClassForFridgeType(EFridgeType FridgeType)
{
	switch (FridgeType)
	{
		case EFridgeType::SingleDoor:
			return SingleDoorFridgePlayerClass;
		case EFridgeType::DoubleDoor:
			return DoubleDoorFridgePlayerClass;
		case EFridgeType::MiniFridge:
			return MiniFridgePlayerClass;
		//default:
			// return ADefaultPlayer::StaticClass();  // Fallback player class
	}
}
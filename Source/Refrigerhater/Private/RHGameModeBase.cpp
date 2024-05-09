#include "RHGameModeBase.h"

#include "EngineUtils.h"
#include "RHCustomLog.h"
#include "Game/RHGameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RHBasePlayer.h"
#include "Player/RHBasePlayerController.h"
#include "Player/RHBasePlayerState.h"
#include "EosSetup/RHEosPlayerState.h"


ARHGameModeBase::ARHGameModeBase()
{
	PlayerControllerClass = ARHBasePlayerController::StaticClass();
	DefaultPawnClass = ARHBasePlayer::StaticClass();
	PlayerStateClass = ARHBasePlayerState::StaticClass();
	GameStateClass = ARHGameStateBase::StaticClass();

	bStartPlayersAsSpectators = true;
	
	CacheTeamSpawnPoints();
	bUseSeamlessTravel = true;

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

	if (const ARHBasePlayer* NewPlayerPawn = Cast<ARHBasePlayer>(NewPlayer->GetPawn()))
	{
		UE_LOG(LogPlayer, Log, TEXT("Player Connected %s"), *NewPlayerPawn->PlayerName);
	}
}

/**
 * Our custom game start function not to be confused with unreal's own overridable start game related functions in game mode base
 */
void ARHGameModeBase::StartGameCustom()
{
	// Fetch the appropriate team color from GameState and apply it
	ARHGameStateBase* MyGameState = GetGameState<ARHGameStateBase>();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AController* PlayerController = It->Get();
		if (PlayerController && !PlayerController->GetPawn()) 
		{
			// we are manually spawning the players here based on the teams they have selected
			SpawnPlayerAtTeamStart(PlayerController); 
		}
		FTimerHandle TimerHandle; // Set a timer to re-enable collision after a very short delay
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PlayerController, MyGameState]()
		{
			if (ARHBasePlayer* PlayerPawn = Cast<ARHBasePlayer>(PlayerController->GetPawn()))
			{
				if (const ARHBasePlayerState* PlayerState = PlayerController->GetPlayerState<ARHBasePlayerState>())
				{
				
					const FLinearColor TeamColor = (PlayerState->Team == 0) ? MyGameState->TeamAColor : MyGameState->TeamBColor;
					PlayerPawn->SetTeamColor(TeamColor);
				}
			}
		}, 0.5f, false);

	}
	
	UE_LOG(LogPlayer, Log, TEXT("Everyones ready lets go"));
	MyGameState->bArePlayersReady = true; // This will trigger OnRep_PlayersReady on all clients

	//force call on rep ready on the server too!
	MyGameState->OnRep_PlayersReady();
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
	//no longer needed
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
	return ReadyPlayers == NumConnectedPlayers - 1; //temp
}

bool ARHGameModeBase::IsTeamAndFridgeValid(int32 PlayerTeam, const EFridgeType FridgeType)
{
	//can be extended in the future
	return true; 
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

UClass* ARHGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ARHEosPlayerState* playerState = InController->GetPlayerState<ARHEosPlayerState>();

	if(playerState)
		if (playerState->GetChosenCharacter())
			return playerState->GetChosenCharacter();

	return Super::GetDefaultPawnClassForController(InController);
}

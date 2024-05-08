#include "RHGameModeBase.h"
#include "RHCustomLog.h"
#include "Player/RHBasePlayer.h"
#include "Player/RHBasePlayerController.h"
#include "RHEosPlayerState.h"


ARHGameModeBase::ARHGameModeBase()
{
	
	PlayerControllerClass = ARHBasePlayerController::StaticClass();
	DefaultPawnClass = ARHBasePlayer::StaticClass();
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
		// Now call the function on the pawn that updates all clients with the initial state
		//NewPlayerPawn->UpdateAllClientsWithInitialState();
	}
	else
	{
		UE_LOG(LogPlayer, Error, TEXT("Failed to find the player pawn"));
	}
}

UClass* ARHGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ARHEosPlayerState* playerState = InController->GetPlayerState<ARHEosPlayerState>();

	if(playerState)
		if (playerState->GetChosenCharacter())
			return playerState->GetChosenCharacter();

	return Super::GetDefaultPawnClassForController(InController);
}

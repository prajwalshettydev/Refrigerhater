#include "RHLobbyGameMode.h"
#include "RHEOSGameState.h"
#include "RHEosGameInstance.h"

ARHLobbyGameMode::ARHLobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ARHLobbyGameMode::InitGameState()
{
	Super::InitGameState();

	ARHEOSGameState* gameState = Cast<ARHEOSGameState>(GameState);
	URHEosGameInstance* gameInstance = GetGameInstance<URHEosGameInstance>();

	if (gameState && gameInstance)
		gameState->SetSessionName(gameInstance->GetCurrentSessionName());
}

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RHLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void InitGameState() override;
};

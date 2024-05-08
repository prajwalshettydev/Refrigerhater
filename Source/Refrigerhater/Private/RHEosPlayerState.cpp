#include "RHEosPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "RHEOSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "RHCharacterSelectionType.h"

ARHEosPlayerState::ARHEosPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;
}

TSubclassOf<class ACharacter> ARHEosPlayerState::GetChosenCharacter() const
{
	if (chosenChar)
		return chosenChar->charClass;
	return defaultChar;
}

void ARHEosPlayerState::Server_IssueCharacterChoice_Implementation(URHCharacterSelectionType* newCharChoice)
{
	if (newCharChoice == chosenChar)
		return;

	ARHEOSGameState* gameState = Cast<ARHEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!gameState || gameState->IsCharacterChosen(newCharChoice))
		return;

	gameState->UpdateChosenCharacterList(newCharChoice, chosenChar);
	chosenChar = newCharChoice;

	OnChosenCharacterReplicated.Broadcast(chosenChar);
}

bool ARHEosPlayerState::Server_IssueCharacterChoice_Validate(URHCharacterSelectionType* newCharSelection)
{
	return true;
}

void ARHEosPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ARHEosPlayerState, chosenChar, COND_None, REPNOTIFY_Always);
}

void ARHEosPlayerState::CopyProperties(APlayerState* PlayerState)
{
	ARHEosPlayerState* playerState = Cast<ARHEosPlayerState>(PlayerState);
	if (playerState) {
		playerState->chosenChar = chosenChar;
	}
}

void ARHEosPlayerState::OnRep_ChosenCharacter()
{
	OnChosenCharacterReplicated.Broadcast(chosenChar);
}

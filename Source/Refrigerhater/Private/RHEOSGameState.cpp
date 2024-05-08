#include "RHEOSGameState.h"
#include "Net/UnrealNetwork.h"

void ARHEOSGameState::SetSessionName(const FName& sessionNameToSet)
{
	sessionName = sessionNameToSet;
}

const TArray<class URHCharacterSelectionType*>& ARHEOSGameState::GetCharacterChoices() const
{
	return characterChoices;
}

bool ARHEOSGameState::IsCharacterChosen(const URHCharacterSelectionType* characterToCheck) const
{
	return chosenCharacters.Contains(characterToCheck);
}

void ARHEOSGameState::OnRep_SessionName()
{
	OnSessionNameRep.Broadcast(sessionName);
}

void ARHEOSGameState::NetMulticast_UpdatedChosenCharacters_Implementation(const URHCharacterSelectionType* newChoice, const URHCharacterSelectionType* oldChoice)
{
	OnCharacterChosenRep.Broadcast(newChoice, oldChoice);
}

void ARHEOSGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ARHEOSGameState, sessionName, COND_None, REPNOTIFY_Always);
}

void ARHEOSGameState::UpdateChosenCharacterList(const URHCharacterSelectionType* newCharToUpdate, const URHCharacterSelectionType* oldCharToUpdate)
{
	if (!HasAuthority())
		return;

	if (newCharToUpdate == nullptr)
		return;
	chosenCharacters.Add(newCharToUpdate);

	if (oldCharToUpdate == nullptr)
		return;
	chosenCharacters.Remove(oldCharToUpdate);

	NetMulticast_UpdatedChosenCharacters(newCharToUpdate, oldCharToUpdate);
}

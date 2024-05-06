#include "RHEOSGameState.h"
#include "Net/UnrealNetwork.h"

void ARHEOSGameState::SetSessionName(const FName& sessionNameToSet)
{
	sessionName = sessionNameToSet;
}

void ARHEOSGameState::OnRep_SessionName()
{
	OnSessionNameRep.Broadcast(sessionName);
}

void ARHEOSGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ARHEOSGameState, sessionName, COND_None, REPNOTIFY_Always);
}

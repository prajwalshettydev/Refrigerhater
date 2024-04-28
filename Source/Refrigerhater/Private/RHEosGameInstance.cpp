#include "RHEosGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include <Online/OnlineSessionNames.h>

void URHEosGameInstance::Init()
{
	Super::Init();
	onlineSubSystem = IOnlineSubsystem::Get();
	identityPtr = onlineSubSystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &URHEosGameInstance::LoginComplete);

	sessionPtr = onlineSubSystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &URHEosGameInstance::CreateSessionComplete);

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &URHEosGameInstance::FindSessionsComplete);
}

void URHEosGameInstance::LoginComplete(int noOfPlayers, bool bWasSuccessful, const FUniqueNetId& userID, const FString& error)
{
	if (bWasSuccessful){ 
		UE_LOG(LogTemp, Warning, TEXT("LOGGED IN"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("LOGIN FAILED: %s"), *error)
	}
}

void URHEosGameInstance::CreateSessionComplete(FName name, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("SESSION CREATED"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SESSION CREATION FAILED"))
	}
}

void URHEosGameInstance::FindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults) {
			UE_LOG(LogTemp, Warning, TEXT("FOUND LOBBY ID: %s"), *lobbyFound.GetSessionIdStr())
		}	
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("LOBBIES NOT FOUND"))
	}
}

void URHEosGameInstance::EOSLogin()
{
	if (!identityPtr)
		return;

	FOnlineAccountCredentials credentials;
	credentials.Type = "accountPortal";
	credentials.Id = "";
	credentials.Token = "";

	identityPtr->Login(0, credentials);
}

void URHEosGameInstance::CreateSession()
{
	if (!sessionPtr)
		return;

	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowInvites = true;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.NumPublicConnections = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = false;

	// Arbitrary data, readable on client
	sessionSettings.Set(FName("LobbyName"), FString("TestLob"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	sessionPtr->CreateSession(0, "RHGameSession", sessionSettings);

	// Load the Lobby Level if not loaded
	if(!LobbyLevel.IsValid())
		LobbyLevel.LoadSynchronous();

	if (LobbyLevel.IsValid()) {

		const FName levelName = FName(*FPackageName::ObjectPathToPackageName(LobbyLevel.ToString()));
		GetWorld()->ServerTravel(levelName.ToString() + "?listen");
	}
}

void URHEosGameInstance::FindSession()
{
	if (!sessionPtr)
		return;

	sessionSearch = MakeShareable(new FOnlineSessionSearch);
	sessionSearch->bIsLanQuery = false;
	sessionSearch->MaxSearchResults = 100;
	sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	sessionPtr->FindSessions(0, sessionSearch.ToSharedRef());
}

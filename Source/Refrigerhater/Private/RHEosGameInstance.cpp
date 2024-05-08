#include "RHEosGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &URHEosGameInstance::FindSessionComplete);
	sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &URHEosGameInstance::JoinSessionComplete);
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

void URHEosGameInstance::FindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && sessionSearch->SearchResults.Num() > 0) {
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults) {
			FString lobbyName = GetSessionName(lobbyFound);
			UE_LOG(LogTemp, Warning, TEXT("FOUND LOBBY ID: %s"), *lobbyFound.GetSessionIdStr())
			UE_LOG(LogTemp, Warning, TEXT("LOBBY NAME: %s"), *lobbyName)
		}

		onSearchCompleted.Broadcast(sessionSearch->SearchResults);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("LOBBIES NOT FOUND"))
	}
}

void URHEosGameInstance::JoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type resultType)
{
	if (resultType == EOnJoinSessionCompleteResult::Success) {
		FString outURL;
		sessionPtr->GetResolvedConnectString(sessionName, outURL);
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(outURL, ETravelType::TRAVEL_Absolute);
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

void URHEosGameInstance::CreateSession(const FName sessionName)
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
	sessionSettings.NumPublicConnections = 4;

	sessionSettings.Set(sessionNameID, sessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	currentSessionName = sessionName;

	sessionPtr->CreateSession(0, sessionName, sessionSettings);
	LoadLevelListen(lobbyLevel);
}

void URHEosGameInstance::LoadLevelListen(TSoftObjectPtr<UWorld> levelToLoad)
{
	// Load the Level if not already loaded
	if (!levelToLoad.IsValid())
		levelToLoad.LoadSynchronous();

	if (levelToLoad.IsValid()) {

		const FName levelName = FName(*FPackageName::ObjectPathToPackageName(levelToLoad.ToString()));
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

FString URHEosGameInstance::GetSessionName(const FOnlineSessionSearchResult result) const
{
	FString outVal = {"NoSesssionName"};
	result.Session.SessionSettings.Get(sessionNameID, outVal);
	return outVal;
}

void URHEosGameInstance::JoinLobbyByIndex(int index)
{
	if (index < 0 || index >= sessionSearch->SearchResults.Num())
		return;

	FOnlineSessionSearchResult& lobbyToJoin = sessionSearch->SearchResults[index];

	FString lobbyToJoinName = GetSessionName(lobbyToJoin);
	if (sessionPtr)
		sessionPtr->JoinSession(0, FName(lobbyToJoinName), lobbyToJoin);
}

void URHEosGameInstance::StartGame()
{
	LoadLevelListen(gameLevel);
}

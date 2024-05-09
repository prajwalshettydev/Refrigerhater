#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "RHEosGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchCompleted, const TArray<FOnlineSessionSearchResult>&) // Our SearchResult for lobbies
/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHEosGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FOnSessionSearchCompleted onSearchCompleted;

	UFUNCTION(BlueprintCallable)
	void EOSLogin();

	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName sessionName);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	FORCEINLINE FName GetSessionNameID() const { return sessionNameID; }
	FORCEINLINE FName GetCurrentSessionName() const { return currentSessionName; }

	FString GetSessionName(const FOnlineSessionSearchResult result) const;

	void JoinLobbyByIndex(int index);

	void StartGame();
protected:
	virtual void Init() override;
private:
	class IOnlineSubsystem* onlineSubSystem;
	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionPtr;

	void LoginComplete(int noOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void CreateSessionComplete(FName name, bool bWasSuccessful);
	void FindSessionComplete(bool bWasSuccessful);
	void JoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type resultType);
	void LoadLevelListen(TSoftObjectPtr<UWorld> levelToLoad);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> gameLevel;	

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> lobbyLevel;

	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
	const FName sessionNameID{ "SessionNameID" };
	FName currentSessionName;
};

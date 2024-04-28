#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RHEosGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHEosGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void EOSLogin();
	UFUNCTION(BlueprintCallable)
	void CreateSession();
	UFUNCTION(BlueprintCallable)
	void FindSession();
protected:
	virtual void Init() override;
private:
	class IOnlineSubsystem* onlineSubSystem;
	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionPtr;

	void LoginComplete(int noOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void CreateSessionComplete(FName name, bool bWasSuccessful);
	void FindSessionsComplete(bool bWasSuccessful);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> LobbyLevel;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
};

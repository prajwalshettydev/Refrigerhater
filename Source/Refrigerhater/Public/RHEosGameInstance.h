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
protected:
	virtual void Init() override;
private:
	class IOnlineSubsystem* onlineSubSystem;
	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;

	void LoginComplete(int noOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};

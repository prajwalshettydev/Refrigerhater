#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RHEOSGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionNameRep, const FName&, name);
UCLASS()
class REFRIGERHATER_API ARHEOSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	FOnSessionNameRep OnSessionNameRep;
	FORCEINLINE FName GetSessionName() const { return sessionName; }
	void SetSessionName(const FName& sessionNameToSet);
private:
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName sessionName;

	UFUNCTION()
	void OnRep_SessionName();
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty >& OutLifetimeProps) const;
};

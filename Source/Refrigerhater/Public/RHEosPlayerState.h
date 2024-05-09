#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RHEosPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChosenCharacterReplicated, const URHCharacterSelectionType*, newSelection);
/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHEosPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ARHEosPlayerState();
	FOnChosenCharacterReplicated OnChosenCharacterReplicated;
	TSubclassOf<class ACharacter> GetChosenCharacter() const;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_IssueCharacterChoice(class URHCharacterSelectionType* newCharChoice);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACharacter> defaultChar;
protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_ChosenCharacter)
	class URHCharacterSelectionType* chosenChar;

	UFUNCTION()
	void OnRep_ChosenCharacter();
};

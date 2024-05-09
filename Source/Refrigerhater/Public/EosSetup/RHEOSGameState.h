#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RHEOSGameState.generated.h"

/**
 * 
 */
class URHCharacterSelectionType;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionNameRep, const FName&, name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterChosenRep, const URHCharacterSelectionType*, newChoice, const URHCharacterSelectionType*, oldChoice);

UCLASS()
class REFRIGERHATER_API ARHEOSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	FOnSessionNameRep OnSessionNameRep;
	FOnCharacterChosenRep OnCharacterChosenRep;
	FORCEINLINE FName GetSessionName() const { return sessionName; }
	void SetSessionName(const FName& sessionNameToSet);

	const TArray<URHCharacterSelectionType*>& GetCharacterChoices() const;

	bool IsCharacterChosen(const URHCharacterSelectionType* characterToCheck) const;
	void UpdateChosenCharacterList(const URHCharacterSelectionType* newCharToUpdate, const URHCharacterSelectionType* oldCharToUpdate);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty >& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName sessionName;

	UFUNCTION()
	void OnRep_SessionName();

	UPROPERTY(EditDefaultsOnly)
	TArray<URHCharacterSelectionType*> characterChoices;
	TArray<const URHCharacterSelectionType*> chosenCharacters;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_UpdatedChosenCharacters(const URHCharacterSelectionType* newChoice, const URHCharacterSelectionType* oldChoice);
};

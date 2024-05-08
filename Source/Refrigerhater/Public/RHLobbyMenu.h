// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RHLobbyMenu.generated.h"

/**
 *
 */
UCLASS()
class REFRIGERHATER_API URHLobbyMenu : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* lobbyNameDisplay;

	UPROPERTY(meta=(BindWidget))
	class UListView* playerListDisplay;	
	
	UPROPERTY(meta=(BindWidget))
	class UTileView* characterListDisplay;	
	
	UPROPERTY(meta=(BindWidget))
	class UButton* startGameButton;

	UPROPERTY()
	class ARHEOSGameState* gameState;

	UPROPERTY()
	class ARHEosPlayerState* playerState;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARHCharacterDisplay> characterDisplayClass;

	ARHCharacterDisplay* displayCharacter;

	UFUNCTION()
	void SessionNameRep(const FName& newSessionName);

	UFUNCTION()
	void CharacterChoiceReplicated(const class URHCharacterSelectionType* newChoice, const class URHCharacterSelectionType* oldChoice);

	UFUNCTION()
	void LoadGame();

	FTimerHandle playerListUpdateTimer;

	void RefreshPlayerList();

	void PlayerHasChosenCharacter(UObject* listIem);
};

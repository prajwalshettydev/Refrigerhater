#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "RHMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	class URHEosGameInstance* gameInstance;
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* loginButton;	
	UPROPERTY(meta = (BindWidget))
	class UButton* createLobbyButton;	
	UPROPERTY(meta = (BindWidget))	
	class UButton* joinLobbyButton;	
	UPROPERTY(meta = (BindWidget))
	class UButton* findLobbyButton;
	UPROPERTY(meta = (BindWidget))
	class UEditableText* lobbyNameTextBox;	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* lobbyNameList;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URHLobbyEntry> lobbyEntryClass;

	UFUNCTION()
	void LoginButtonOnClick();
	UFUNCTION()
	void CreateLobbyButtonOnClick();	
	UFUNCTION()
	void FindLobbyButtonOnClick();	
	UFUNCTION()
	void JoinLobbyButtonOnClick();
	UFUNCTION()
	void LobbyNameChanged(const FText& name);
	UFUNCTION()
	void LobbySelected(int index);

	void SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& lobbySearchResults);

	int selectedLobbyIndex = -1;
};
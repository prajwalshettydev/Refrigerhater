#include "EosSetup/RHMainMenu.h"
#include "EosSetup/RHEosGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "EosSetup/RHLobbyEntry.h"

void URHMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	gameInstance = GetGameInstance<URHEosGameInstance>();

	loginButton->OnClicked.AddDynamic(this, &URHMainMenu::LoginButtonOnClick);

	createLobbyButton->SetIsEnabled(false);
	createLobbyButton->OnClicked.AddDynamic(this, &URHMainMenu::CreateLobbyButtonOnClick);
	findLobbyButton->SetIsEnabled(false);
	findLobbyButton->OnClicked.AddDynamic(this, &URHMainMenu::FindLobbyButtonOnClick);
	joinLobbyButton->SetIsEnabled(false);
	joinLobbyButton->OnClicked.AddDynamic(this, &URHMainMenu::JoinLobbyButtonOnClick);

	lobbyNameTextBox->OnTextChanged.AddDynamic(this, &URHMainMenu::LobbyNameChanged);

	gameInstance->onSearchCompleted.AddUObject(this, &URHMainMenu::SessionSearchCompleted);
}

void URHMainMenu::CreateLobbyButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->CreateSession(FName(lobbyNameTextBox->Text.ToString()));
}

void URHMainMenu::FindLobbyButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->FindSession();
}

void URHMainMenu::JoinLobbyButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->JoinLobbyByIndex(selectedLobbyIndex);
}

void URHMainMenu::LobbyNameChanged(const FText& name)
{
	createLobbyButton->SetIsEnabled(!name.IsEmpty());
}

void URHMainMenu::LobbySelected(int index)
{
	selectedLobbyIndex = index;
	joinLobbyButton->SetIsEnabled(selectedLobbyIndex != -1);
}

void URHMainMenu::SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& lobbySearchResults)
{
	lobbyNameList->ClearChildren();
	int i = 0;
	for (const FOnlineSessionSearchResult& result : lobbySearchResults) {
		FString sessionName = gameInstance->GetSessionName(result);
		URHLobbyEntry* lobbyEntry = CreateWidget<URHLobbyEntry>(lobbyNameList, lobbyEntryClass);
		lobbyEntry->InitLobbyEntry(FName(sessionName), i);
		lobbyNameList->AddChild(lobbyEntry);
		lobbyEntry->onLobbySelected.AddDynamic(this, &URHMainMenu::LobbySelected);
		++i;
	}
}

void URHMainMenu::LoginButtonOnClick()
{
	if (!gameInstance)
		return;

	gameInstance->EOSLogin();
	findLobbyButton->SetIsEnabled(true);
}
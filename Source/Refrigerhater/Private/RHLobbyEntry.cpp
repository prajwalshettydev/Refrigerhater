#include "RHLobbyEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void URHLobbyEntry::NativeConstruct()
{
	Super::NativeConstruct();

	lobbyButton->OnClicked.AddDynamic(this, &URHLobbyEntry::LobbyButtonOnClick);
}

void URHLobbyEntry::InitLobbyEntry(const FName& lobbyEntryName, int lobbyEntryIndex)
{
	lobbyNameText->SetText(FText::FromName(lobbyEntryName));
	lobbyIndex = lobbyEntryIndex;
}

void URHLobbyEntry::LobbyButtonOnClick()
{
	onLobbySelected.Broadcast(lobbyIndex);
}

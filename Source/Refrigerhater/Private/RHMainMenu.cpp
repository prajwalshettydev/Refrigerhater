#include "RHMainMenu.h"
#include "RHEosGameInstance.h"
#include "Components/Button.h"

void URHMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	gameInstance = GetGameInstance<URHEosGameInstance>();

	loginButton->OnClicked.AddDynamic(this, &URHMainMenu::LoginButtonOnClick);
	createLobbyButton->OnClicked.AddDynamic(this, &URHMainMenu::CreateLobbyButtonOnClick);
	findLobbyButton->OnClicked.AddDynamic(this, &URHMainMenu::FindLobbyButtonOnClick);
}

void URHMainMenu::CreateLobbyButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->CreateSession();
}

void URHMainMenu::FindLobbyButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->FindSession();
}

void URHMainMenu::LoginButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->EOSLogin();
}
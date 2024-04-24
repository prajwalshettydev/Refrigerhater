#include "RHMainMenu.h"
#include "RHEosGameInstance.h"
#include "Components/Button.h"

void URHMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	gameInstance = GetGameInstance<URHEosGameInstance>();

	loginButton->OnClicked.AddDynamic(this, &URHMainMenu::LoginButtonOnClick);
}

void URHMainMenu::LoginButtonOnClick()
{
	if (!gameInstance)
		return;
	gameInstance->EOSLogin();
}
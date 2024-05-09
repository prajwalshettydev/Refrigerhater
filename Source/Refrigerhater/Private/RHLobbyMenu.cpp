#include "RHLobbyMenu.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "RHEOSGameState.h"
#include "RHEosPlayerState.h"
#include "RHEosGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "RHCharacterSelectionType.h"
#include "RHCharacterEntry.h"
#include "RHCharacterDisplay.h"

void URHLobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();
	gameState = Cast<ARHEOSGameState>(UGameplayStatics::GetGameState(this));
	playerState = GetOwningPlayerState<ARHEosPlayerState>();

	if (!gameState || !playerState)
		return;

	FName lobbyName = gameState->GetSessionName();
	lobbyNameDisplay->SetText(FText::FromName(lobbyName));
	UE_LOG(LogTemp, Warning, TEXT("LOBBY NAME: %s"), *lobbyName.ToString())
		gameState->OnSessionNameRep.AddDynamic(this, &URHLobbyMenu::SessionNameRep);

	playerListDisplay->SetListItems(gameState->PlayerArray);

	GetWorld()->GetTimerManager().SetTimer(playerListUpdateTimer, this, &URHLobbyMenu::RefreshPlayerList, 1, true);

	characterListDisplay->SetListItems(gameState->GetCharacterChoices());
	characterListDisplay->OnItemSelectionChanged().AddUObject(this, &URHLobbyMenu::PlayerHasChosenCharacter);

	gameState->OnCharacterChosenRep.AddDynamic(this, &URHLobbyMenu::CharacterChoiceReplicated);

	FActorSpawnParameters spawnParams;
	APlayerController* owningPC = GetOwningPlayer();
	spawnParams.Owner = owningPC;

	displayCharacter = GetWorld()->SpawnActor<ARHCharacterDisplay>(characterDisplayClass, spawnParams);
	UE_LOG(LogTemp, Warning, TEXT("DISPLAY CHAR: %s"), displayCharacter==nullptr? TEXT("TRUE") : TEXT("FALSE"));
	owningPC->SetViewTarget(displayCharacter);

	playerState->OnChosenCharacterReplicated.AddDynamic(displayCharacter, &ARHCharacterDisplay::SetCharacter);

	startGameButton->OnClicked.AddDynamic(this, &URHLobbyMenu::LoadGame);
}

void URHLobbyMenu::SessionNameRep(const FName& newSessionName)
{
	lobbyNameDisplay->SetText(FText::FromName(newSessionName));
	UE_LOG(LogTemp, Warning, TEXT("LOBBY NAME: %s"), *lobbyNameDisplay->GetText().ToString())
}

void URHLobbyMenu::CharacterChoiceReplicated(const URHCharacterSelectionType* newChoice, const URHCharacterSelectionType* oldChoice)
{
	if (newChoice != nullptr) {
		URHCharacterEntry* newCharEntry = characterListDisplay->GetEntryWidgetFromItem<URHCharacterEntry>(newChoice);
		newCharEntry->SetChosen(true);
	}

	if (oldChoice != nullptr) {
		URHCharacterEntry* oldCharEntry = characterListDisplay->GetEntryWidgetFromItem<URHCharacterEntry>(oldChoice);
		oldCharEntry->SetChosen(false);
	}
}

void URHLobbyMenu::LoadGame()
{
	URHEosGameInstance* gameInstance = GetGameInstance<URHEosGameInstance>();

	if (!gameInstance)
		return;

	gameInstance->StartGame();
}

void URHLobbyMenu::RefreshPlayerList()
{
	if (gameState)
		playerListDisplay->SetListItems(gameState->PlayerArray);
}

void URHLobbyMenu::PlayerHasChosenCharacter(UObject* listIem)
{
	URHCharacterSelectionType* chosenCharacter = Cast<URHCharacterSelectionType>(listIem);
	if (chosenCharacter)
		playerState->Server_IssueCharacterChoice(chosenCharacter);
}

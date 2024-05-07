// Fill out your copyright notice in the Description page of Project Settings.


#include "RHLobbyMenu.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "RHEOSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

void URHLobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();
	gameState = Cast<ARHEOSGameState>(UGameplayStatics::GetGameState(this));

	if (gameState) {

		FName lobbyName = gameState->GetSessionName();
		lobbyNameDisplay->SetText(FText::FromName(lobbyName));
		UE_LOG(LogTemp, Warning, TEXT("LOBBY NAME: %s"), *lobbyName.ToString())
		gameState->OnSessionNameRep.AddDynamic(this, &URHLobbyMenu::SessionNameRep);

		playerListDisplay->SetListItems(gameState->PlayerArray);

		GetWorld()->GetTimerManager().SetTimer(playerListUpdateTimer, this, &URHLobbyMenu::RefreshPlayerList, 1, true);
	}


}

void URHLobbyMenu::SessionNameRep(const FName& newSessionName)
{
	lobbyNameDisplay->SetText(FText::FromName(newSessionName));
	UE_LOG(LogTemp, Warning, TEXT("LOBBY NAME: %s"), *lobbyNameDisplay->GetText().ToString())
}

void URHLobbyMenu::RefreshPlayerList()
{
	if(gameState)
		playerListDisplay->SetListItems(gameState->PlayerArray);
}

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
	UPROPERTY()
	class ARHEOSGameState* gameState;
	UFUNCTION()
	void SessionNameRep(const FName& newSessionName);

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* lobbyNameDisplay;

	UPROPERTY(meta=(BindWidget))
	class UListView* playerListDisplay;

	FTimerHandle playerListUpdateTimer;

	void RefreshPlayerList();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RHLobbyEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyEntrySelected, int, lobbyEntryIndex);
/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHLobbyEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitLobbyEntry(const FName& lobbyEntryName, int lobbyEntryIndex);
	FOnLobbyEntrySelected onLobbySelected;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* lobbyButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* lobbyNameText;

	UFUNCTION()
	void LobbyButtonOnClick();

	int lobbyIndex = -1;
};

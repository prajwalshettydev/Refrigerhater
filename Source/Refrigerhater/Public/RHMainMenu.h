#pragma once

#include "CoreMinimal.h"
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
	class UButton* findLobbyButton;

	UFUNCTION()
	void LoginButtonOnClick();
	UFUNCTION()
	void CreateLobbyButtonOnClick();	
	UFUNCTION()
	void FindLobbyButtonOnClick();
};
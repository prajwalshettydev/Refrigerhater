#include "RHEosGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void URHEosGameInstance::Init()
{
	Super::Init();
	onlineSubSystem = IOnlineSubsystem::Get();
	identityPtr = onlineSubSystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &URHEosGameInstance::LoginComplete);
}

void URHEosGameInstance::LoginComplete(int noOfPlayers, bool bWasSuccessful, const FUniqueNetId& userID, const FString& error)
{
	if (bWasSuccessful){ 
		UE_LOG(LogTemp, Warning, TEXT("LOGGED IN"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("LOGIN FAILED: %s"), *error)
	}
}

void URHEosGameInstance::EOSLogin()
{
	if (!identityPtr)
		return;

	FOnlineAccountCredentials credentials;
	credentials.Type = "accountPortal";
	credentials.Id = "";
	credentials.Token = "";

	identityPtr->Login(0, credentials);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "RHGameModeBase.h"

#include "RHCustomLog.h"
#include "Player/RHBasePlayer.h"
#include "Player/RHBasePlayerController.h"


ARHGameModeBase::ARHGameModeBase()
{
	
	PlayerControllerClass = ARHBasePlayerController::StaticClass();
	DefaultPawnClass = ARHBasePlayer::StaticClass();


	// Blueprinted Version, relies on the asset path obtained from the editor
	static ConstructorHelpers::FClassFinder<ARHBasePlayer> PlayerPawnClassFinder(TEXT("/Game/Player/BP_SingleDoorFridge.BP_SingleDoorFridge"));
	if (PlayerPawnClassFinder.Succeeded())
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
	}
	else
	{
		UE_LOG(LogPlayer, Error, TEXT("Failed to find the player pawn bp class"));
	}
}

void ARHGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ARHGameModeBase::StartPlay()
{
	Super::StartPlay();
}

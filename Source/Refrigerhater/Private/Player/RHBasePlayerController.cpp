// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\Player\RHBasePlayerController.h"

#include "RHCustomLog.h"
#include "Player/RHBasePlayer.h"

void ARHBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cast the possessed pawn to the custom player pawn class

	if (const ARHBasePlayer* PlayerPawn = Cast<ARHBasePlayer>(InPawn))
	{
		UE_LOG(LogPlayer, Log, TEXT("Player ARHBasePlayer is now possesed"));
		// Perform custom logic with your custom player pawn
	}
	else
	{
		UE_LOG(LogPlayer, Error, TEXT("Player not possed?"));
	}
	
}

ARHBasePlayerController::ARHBasePlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ARHBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// if (IsLocalController())
	// {
	// 	if (IsLocalPlayerController())
	// 	{
	// 		if (HasAuthority())
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is the Server"));
	// 		}
	// 		else
	// 		{
	// 			static int ClientID = 1;
	// 			FString ClientText = FString::Printf(TEXT("This is Client %d"), ClientID++);
	// 			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, ClientText);
	// 		}
	// 	}
	// }
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RHGameModeBase.generated.h"

//
// class USSaveGame;
/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARHGameModeBase();
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};

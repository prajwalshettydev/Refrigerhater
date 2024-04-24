// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RHBasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHBasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void OnPossess(APawn* InPawn) override;

public:
	ARHBasePlayerController();
	void BeginPlay();
};

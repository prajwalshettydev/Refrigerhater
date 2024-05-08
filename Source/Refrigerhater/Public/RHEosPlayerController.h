// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RHEosPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHEosPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnRep_PlayerState"))
	void BP_OnRep_PlayerState();
};

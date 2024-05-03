// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RHGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHGameStateBase : public AGameState
{
	GENERATED_BODY()

public:
	// Array to hold readiness of each player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Player State")
	TArray<bool> PlayerReadiness;

	// Replicate player readiness
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

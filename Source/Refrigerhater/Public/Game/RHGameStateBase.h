// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RHGameStateBase.generated.h"

//more on delegates in readme!! 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	UFUNCTION()
	void OnRep_PlayersReady() const;
	
	// Replicate player readiness
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_PlayersReady)
	bool bArePlayersReady;

	// Delegate instance
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnAllPlayersReady OnAllPlayersReady;
	
	// Array to hold readiness of each player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Player State")
	TArray<bool> PlayerReadiness;

};

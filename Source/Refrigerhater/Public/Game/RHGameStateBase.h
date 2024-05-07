// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Resources/RHResourceBase.h"
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
	
	ARHGameStateBase();
private:
	float TimeSinceLastReplication = 0.0f;
	float ReplicationInterval = 1.0f;
	
	
	// Replicate player readiness
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void Tick(float DeltaSeconds) override;
	
public:
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 TeamAScore;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 TeamBScore;

	UPROPERTY(EditDefaultsOnly, Category = "Resource Points")
	TMap<EResourceType, EResourceCategory> ResourceCategories;
	
    void AddScore(EResourceType ResourceType, bool IsTeamA);
	
	UFUNCTION()
	void OnRep_PlayersReady() const;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayersReady)
	bool bArePlayersReady; //or has the game started?

	// Current game time
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	float GameTimeSeconds;

	// Delegate instance
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnAllPlayersReady OnAllPlayersReady;
	
	// Array to hold readiness of each player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Player State")
	TArray<bool> PlayerReadiness;
	
	UFUNCTION(BlueprintCallable)
	FString GetFormattedGameTime() const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/RHBasePlayerState.h"
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

	
private:
	// Stores player choices for teams and fridge types
	TMap<APlayerState*, int32> PlayerTeamAssignments;
	TMap<APlayerState*, FString> PlayerFridgeSelections;

	// Checks if all players are ready and valid
	bool AreAllPlayersReady();

	// Validates team and fridge type selections
	bool IsTeamAndFridgeValid(int32 PlayerTeam, const FString& FridgeType) const;
	//UMyWidget

protected:
	// Arrays to store spawn points for each team

	UPROPERTY()
	TArray<AActor*> Team1SpawnPoints;
	UPROPERTY()
	TArray<AActor*> Team2SpawnPoints;

	// Pawn class references for fridge types
	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> SingleDoorFridgePlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> DoubleDoorFridgePlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> MiniFridgePlayerClass;

public:
	ARHGameModeBase();
	void CacheTeamSpawnPoints();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void StartGame();
	void SpawnPlayerAtTeamStart(AController* Controller);

	// RPC function to receive player readiness and selection data
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayerReady(APlayerController* PlayerController, int32 PlayerTeam, const FString& FridgeType);

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void RestartPlayerAtPlayerStart(AController* Controller, AActor* StartSpot) override;
	UClass* GetPlayerClassForFridgeType(EFridgeType FridgeType);


	//Since ChoosePlayerStart is defined with UFUNCTION(BlueprintNativeEvent, Category=Game), this means we need to use the _Implementation suffix in your override implementation.
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};

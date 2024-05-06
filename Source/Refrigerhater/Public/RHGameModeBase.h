// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/RHBasePlayerState.h"
#include "RHGameModeBase.generated.h"


/**
 * base!! 
 */
UCLASS()
class REFRIGERHATER_API ARHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	// Stores player choices for teams and fridge types
	TMap<APlayerState*, int32> PlayerTeamAssignments;
	TMap<APlayerState*, EFridgeType> PlayerFridgeSelections;
	
	UPROPERTY()
	TArray<AActor*> Team1SpawnPoints;
	UPROPERTY()
	TArray<AActor*> Team2SpawnPoints;
	
protected:
	// Pawn class references for fridge types
	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> SingleDoorFridgePlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> DoubleDoorFridgePlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player Classes")
	TSubclassOf<APawn> MiniFridgePlayerClass;


private:
	bool AreAllPlayersReady();
	static bool IsTeamAndFridgeValid(int32 PlayerTeam, const EFridgeType FridgeType);
	void CacheTeamSpawnPoints();
	void SpawnPlayerAtTeamStart(AController* Controller);
	UClass* GetPlayerClassForFridgeType(EFridgeType FridgeType) const;

protected:
	//Overrides
	
	/**
	 * Since ChoosePlayerStart is defined with UFUNCTION(BlueprintNativeEvent, Category=Game),
	 * this means we need to use the _Implementation suffix in your override implementation.
	 * @param Player 
	 * @return 
	 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void RestartPlayerAtPlayerStart(AController* Controller, AActor* StartSpot) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
public:
	ARHGameModeBase();

	UFUNCTION(BlueprintCallable)
	void StartGameCustom();
	
	UFUNCTION(BlueprintCallable)
	void OnSpecificPlayerIsReady(const APlayerController* PlayerController, int32 PlayerTeam, const EFridgeType FridgeType);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void AddPointsForTeam(const FString& ResourceType, int32 Quantity, int32 TeamID);



};

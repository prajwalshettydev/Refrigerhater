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
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayerReady(int32 SelectedTeam, const EFridgeType FridgeType);

	// In ARHPlayerController.h
	UFUNCTION(BlueprintCallable)
	void CallServerPlayerReady(int32 SelectedTeam, const EFridgeType FridgeType);

	UFUNCTION(BlueprintCallable)
	void PrepareAndCallServerPlayerReady(const FString& TeamName, const FString& FridgeTypeName);
	
	ARHBasePlayerController();
	
	void BeginPlay();
};

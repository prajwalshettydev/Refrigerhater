// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RHBasePlayerState.generated.h"


UENUM(BlueprintType)
enum class EFridgeType : uint8
{
	SingleDoor,
	DoubleDoor,
	MiniFridge
};

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API ARHBasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	int32 Team = 1;
	EFridgeType SelectedFridgeType;
};

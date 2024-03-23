// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RHBasePlayer.h"
#include "RHDoubleDoorFridge.generated.h"

UCLASS()
class REFRIGERHATER_API ARHDoubleDoorFridge : public ARHBasePlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARHDoubleDoorFridge();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

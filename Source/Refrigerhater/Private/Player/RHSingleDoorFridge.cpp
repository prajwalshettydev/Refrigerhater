// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RHSingleDoorFridge.h"


// Sets default values
ARHSingleDoorFridge::ARHSingleDoorFridge()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARHSingleDoorFridge::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARHSingleDoorFridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARHSingleDoorFridge::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


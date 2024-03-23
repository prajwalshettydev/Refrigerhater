// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RHMiniFridge.h"


// Sets default values
ARHMiniFridge::ARHMiniFridge()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARHMiniFridge::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARHMiniFridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARHMiniFridge::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


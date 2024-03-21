// Fill out your copyright notice in the Description page of Project Settings.


#include "Refrigerhater/Public/DummyObjcet.h"


// Sets default values
ADummyObjcet::ADummyObjcet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADummyObjcet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADummyObjcet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


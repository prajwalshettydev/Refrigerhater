// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHResourceBase.h"

/* so each resource can spawn, get grabbed, vanish, allot.
 * 
 *
 */

// Sets default values
ARHResourceBase::ARHResourceBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARHResourceBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARHResourceBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


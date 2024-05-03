// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RHGameStateBase.h"

#include "Net/UnrealNetwork.h"


void ARHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARHGameStateBase, PlayerReadiness);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RHBasePlayerState.h"

#include "Net/UnrealNetwork.h"

void ARHBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARHBasePlayerState, Team);
}

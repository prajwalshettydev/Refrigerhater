// Fill out your copyright notice in the Description page of Project Settings.


#include "RHPlayerEntry.h"
#include "RHEosPlayerState.h"
#include "Components/TextBlock.h"

void URHPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	ARHEosPlayerState* playerState = Cast<ARHEosPlayerState>(ListItemObject);

	if (playerState) {
		playerNameDisplay->SetText(FText::FromString(playerState->GetPlayerName()));
	}
}

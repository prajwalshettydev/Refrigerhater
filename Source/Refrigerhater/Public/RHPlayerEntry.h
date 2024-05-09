// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RHPlayerEntry.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	 virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* playerNameDisplay;
};

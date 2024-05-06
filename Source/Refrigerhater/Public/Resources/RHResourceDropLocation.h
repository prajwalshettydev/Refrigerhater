// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHResourceDropLocation.generated.h"

UCLASS()
class REFRIGERHATER_API ARHResourceDropLocation : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARHResourceDropLocation();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* DropZone;

	UFUNCTION()
	void OnResourceDropped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
	                       bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void NotifyDropReceived(const FString& ResourceType, int32 Quantity);
};

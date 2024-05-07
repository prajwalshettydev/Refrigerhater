// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHResourceDropLocation.generated.h"

UCLASS()
class REFRIGERHATER_API ARHResourceDropLocation : public AActor
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere)
	float MovementRadius = 4500.0f;  // Radius of the circle
	
	UPROPERTY(EditAnywhere)
	float MovementSpeed = 10.0f;    // Speed of movement in degrees per second
	
	UPROPERTY(EditAnywhere)
	float CurrentAngleDegrees = 0.0f;  // Current angle in the movement circle
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* DropZone;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UTextRenderComponent* TextComponent;
	
public:
	// Sets default values for this actor's properties
	ARHResourceDropLocation();

protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
	FVector GetLevelCenter() const;

public:

	UFUNCTION()
	void OnResourceDropped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
	                       bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void NotifyDropReceived(const FString& ResourceType, int32 Quantity);
};

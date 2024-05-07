// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "RHResourceBase.generated.h"

UCLASS()
class REFRIGERHATER_API ARHResourceBase : public AActor
{
	GENERATED_BODY()
	
    float BaseZ; // Base Z position of the arrow
    bool bGoingUp; // Direction control
    float MovementSpeed; // Speed of the arrow's movement
    float MovementRange; // Range of the movement (amplitude)
	
public: 
	// Sets default values for this actor's properties
	ARHResourceBase();
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* ArrowComponent;

protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public: 

	// Function to handle the resource pickup
	UFUNCTION()
	void OnPickedUpBy(APawn* Pawn);
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

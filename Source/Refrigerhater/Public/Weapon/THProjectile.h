// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "THProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class REFRIGERHATER_API ATHProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATHProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, ...);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// Function called when projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Server function to handle damage
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDealDamage(APawn* HitPawn, float DamageAmount);
    
	// Your projectile's damage amount
	//UPROPERTY(EditDefaultsOnly, Category = "Damage")
	//float DamageAmount;

private:
	// Damage value for the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 50.0f;
};

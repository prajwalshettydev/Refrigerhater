// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "RHProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class REFRIGERHATER_API ARHProjectile : public AActor
{
	GENERATED_BODY()
	
	// Damage value for the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 10.0f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;
	
protected:
	virtual void BeginPlay() override;

public:
	ARHProjectile();
	
	UFUNCTION()
	void HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );
	
};

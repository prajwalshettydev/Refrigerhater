// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "THProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class REFRIGERHATER_API ATHProjectile : public AActor
{
	GENERATED_BODY()
	
	// Damage value for the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 50.0f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;
	
protected:
	virtual void BeginPlay() override;

public:
	ATHProjectile();
	
	UFUNCTION()
	void HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );
	
};

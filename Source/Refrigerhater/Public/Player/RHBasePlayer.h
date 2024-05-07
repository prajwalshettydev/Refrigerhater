// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Weapon/RHProjectile.h"
#include "RHBasePlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawn);

class URHWorldWidget;
class UWidgetComponent;
class UCameraComponent;

UCLASS()
class REFRIGERHATER_API ARHBasePlayer : public ACharacter
{
	GENERATED_BODY()
	
	URHWorldWidget* ActiveHealthBar;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	FLinearColor PlayerColor;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Config")
	USkeletalMeshComponent* PlayerMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Config")
	UMaterialInstance* PlayerMaterialInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* NameTagComponent;
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class URHInputConfigData* InputActions;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<ARHProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);
	
public:
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerName)
	FString PlayerName;
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnPlayerRespawn OnPlayerRespawn;
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnPlayerDead OnPlayerDead;
	
	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 400;
	
	// Resource inventory map
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TMap<FString, int32> ResourceInventory;

	// Max inventory size
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	int32 MaxInventorySize = 10;

	// Player resource capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceCapacity;
	
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireWeapon(const FVector& Direction);
	
	UFUNCTION()
	void OnRep_PlayerName();

	UFUNCTION()
	void OnRep_PlayerColor();

	ARHBasePlayer();

private:
	UFUNCTION(BlueprintCallable)
	void InitializeNameTag();
	void ProjectileSpawn(const FVector& Direction) const;
	bool AddResource(const FString& ResourceType, int32 Amount);
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Tap(const FInputActionValue& InputActionValue);
	void Generate(const FInputActionValue& InputActionValue);
	void MoveUpDown(const FInputActionValue& InputActionValue);
	void ChangeMoveSpeed(const FInputActionValue& InputActionValue);
	void HandleDeath();
	void Respawn();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Function to fire weapon
	UFUNCTION(BlueprintCallable)
	void FireWeapon(const FVector& Direction);

	// Handle resource pickup
	void HandleResourcePickup(const FString& ResourceType, int32 Amount);
};

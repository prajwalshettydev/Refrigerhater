// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Weapon/THProjectile.h"
#include "RHBasePlayer.generated.h"

class UWidgetComponent;
class UCameraComponent;
UCLASS()
class REFRIGERHATER_API ARHBasePlayer : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float Health;

	// Player resource capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceCapacity;

	// Mesh Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Config")
	USkeletalMeshComponent* PlayerMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Config")
	UMaterialInstance* PlayerMaterialInstance;
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void ProjectileSpawn(const FVector& Direction);
	void ReceiveDamage(float DamageAmount);
	UFUNCTION()
	void OnRep_Health();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireWeapon(const FVector& Direction);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireWeapon(const FVector& Direction);

	// Replicated properties for player name and color
	UPROPERTY(ReplicatedUsing=OnRep_PlayerName)
	FString PlayerName;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	FLinearColor PlayerColor;

	UFUNCTION()
	void OnRep_PlayerName();

	UFUNCTION()
	void OnRep_PlayerColor();

	// Components for displaying the name
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* NameTagComponent;
	
	// Sets default values for this character's properties
	ARHBasePlayer();

private:
	UFUNCTION(BlueprintCallable)
	void InitializeNameTag();
	
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class URHInputConfigData* InputActions;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<ATHProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Function to fire weapon
	UFUNCTION(BlueprintCallable)
	void FireWeapon(const FVector& Direction);

	// Function to gather resources
	UFUNCTION(BlueprintCallable)
	void GatherResources();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Tap(const FInputActionValue& InputActionValue);
	void Generate(const FInputActionValue& InputActionValue);
	void MoveUpDown(const FInputActionValue& InputActionValue);
	void ChangeMoveSpeed(const FInputActionValue& InputActionValue);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};

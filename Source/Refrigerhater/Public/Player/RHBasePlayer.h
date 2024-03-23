// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "RHBasePlayer.generated.h"

class UCameraComponent;
UCLASS()
class REFRIGERHATER_API ARHBasePlayer : public ACharacter
{
	GENERATED_BODY()

public:

	// Player health
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Health;

	// Player resource capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceCapacity;

	// Mesh Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PlayerMesh;
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	// Sets default values for this character's properties
	ARHBasePlayer();

private:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	
	// Function to fire weapon
	UFUNCTION(BlueprintCallable)
	void FireWeapon();

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

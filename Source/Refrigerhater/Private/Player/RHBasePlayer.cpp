// Fill out your copyright notice in the Description page of Project Settings.


#include "Refrigerhater/Public/Player/RHBasePlayer.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "RHCustomLog.h"
#include "Input/RHInputConfigData.h"


// Sets default values
ARHBasePlayer::ARHBasePlayer()
{
	// Initialize player properties
	Health = 100.f;
	ResourceCapacity = 10; // Default value

	// Setup mesh component
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	RootComponent = PlayerMesh;
	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// // Don't rotate character to camera direction
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;
	// bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	
}

// Called when the game starts or when spawned
void ARHBasePlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARHBasePlayer::FireWeapon()
{
}

void ARHBasePlayer::GatherResources()
{
}

// Called every frame
void ARHBasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARHBasePlayer::Move(const FInputActionValue& InputActionValue)
{

	
	const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		UE_LOG(LogPlayer, Log, TEXT("Input reciegedss: %f, %f"), MoveValue.X,
				   MoveValue.Y);
	}
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), MoveValue.Y);
	
	// x = forward (Red), y is right (Green), z is up (Blue)
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, MoveValue.X);
	//}
}

void ARHBasePlayer::Look(const FInputActionValue& InputActionValue)
{
	
}

void ARHBasePlayer::Tap(const FInputActionValue& InputActionValue)
{
	
}

void ARHBasePlayer::Generate(const FInputActionValue& InputActionValue)
{
}

void ARHBasePlayer::MoveUpDown(const FInputActionValue& InputActionValue)
{
}

void ARHBasePlayer::ChangeMoveSpeed(const FInputActionValue& InputActionValue)
{
}

// Called to bind functionality to input
void ARHBasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* new input system */
	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	
	// // Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
	
	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	//To keep in mind that the check macro should be used for conditions that should never fail during runtime. It is a debugging aid, and in shipping builds, the check is removed. Make sure to fix any issues that cause the check to fail before releasing your game.
	check(PEI && "PlayerInputComponent is not initialized");
	check(InputActions && "PlayerInputActions is not initialized");
	
	// Bind the actions
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ARHBasePlayer::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ARHBasePlayer::Look);
	PEI->BindAction(InputActions->InputTap, ETriggerEvent::Triggered, this, &ARHBasePlayer::Tap);
	PEI->BindAction(InputActions->InputGenerate, ETriggerEvent::Started, this, &ARHBasePlayer::Generate);
	PEI->BindAction(InputActions->InputMoveUpDown, ETriggerEvent::Triggered, this, &ARHBasePlayer::MoveUpDown);
	PEI->BindAction(InputActions->InputMoveSpeed, ETriggerEvent::Triggered, this, &ARHBasePlayer::ChangeMoveSpeed);
	
}

void ARHBasePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARHBasePlayer, Health);
	// Add oth
}
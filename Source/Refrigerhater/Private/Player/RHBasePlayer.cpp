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
#include "RHGameModeBase.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Input/RHInputConfigData.h"
#include "UI/RHWorldWidget.h"
#include "Weapon/RHProjectile.h"

#pragma region init

// List of potential player names
static const TArray<FString> PossiblePlayerNames = {
	TEXT("Maverick"), TEXT("Prajwal"), TEXT("Asheen"),
	TEXT("Phoenix"),
	TEXT("Vortex"),
	TEXT("Blaze"),
	TEXT("Striker"),
	TEXT("Orion"),
	TEXT("Talon"),
	TEXT("Raven"),
	TEXT("Zephyr"),
	TEXT("Drifter"),
	TEXT("Marauder"),
	TEXT("Shadow"),
	TEXT("Fury"),
	TEXT("Raptor"),
	TEXT("Sable"),
	TEXT("Volt"),
	TEXT("Phantom"),
	TEXT("Specter"),
	TEXT("Rogue"),
	TEXT("Eclipse"),
	TEXT("Titan"),
	TEXT("Nomad")
};

// Sets default values
ARHBasePlayer::ARHBasePlayer()
{
	bReplicates = true;

	//Health = 100.f;
	ResourceCapacity = 10;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn")); // Make sure this is correct
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	RootComponent = GetCapsuleComponent();

	// Assuming the parent class has already created a Skeletal Mesh Component named "CharacterMesh0"
	PlayerMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));

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
	CameraBoom->TargetArmLength = 1250.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create the widget component and attach it to the player's root component
	NameTagComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagComponent"));
	NameTagComponent->SetupAttachment(RootComponent);
	NameTagComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameTagComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Adjust as necessary
	NameTagComponent->SetDrawSize(FVector2D(200.0f, 50.0f)); // Adjust as necessary
}

void ARHBasePlayer::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Health = MaxHealth;
		UE_LOG(LogTemp, Warning, TEXT("Player health startnow: %f"), Health);

		// Randomize player color using HSV
		const uint8 Hue = FMath::RandRange(0, 255);
		constexpr uint8 Saturation = 255;
		const uint8 Value = FMath::RandRange(160, 255); // Range from 180 to 255 to ensure brightness

		PlayerColor = FLinearColor::MakeFromHSV8(Hue, Saturation, Value);

		// Delay the initialization of the name tag to ensure all components are ready
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARHBasePlayer::InitializeNameTag, 3.0f, false);
		PlayerName = "";

		CameraBoom->TargetArmLength = 6000.f;
	}


	if (ActiveHealthBar == nullptr)
	{
		ActiveHealthBar = CreateWidget<URHWorldWidget>(GetWorld(), HealthBarWidgetClass);
		//param owningobject here, CANNOT be "this"
		if (ActiveHealthBar)
		{
			ActiveHealthBar->AttachedActor = this;
			ActiveHealthBar->AddToViewport();
			//constructor of the UI widget will be called here. so set it's variable before that
		}
	}
}

void ARHBasePlayer::InitializeNameTag()
{
	FString RandomPlayerName = PossiblePlayerNames[FMath::RandRange(0, PossiblePlayerNames.Num() - 1)] + " ";
	RandomPlayerName.AppendInt(FMath::RandRange(10, 99));
	PlayerName = RandomPlayerName;

	if (HasAuthority())
	{
		//manually call on rep functions for server
		OnRep_PlayerName();
		OnRep_PlayerColor();
	}
}

void ARHBasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion

#pragma region projectile

/**
 * Called on the client, but executed on the server.
 * 
 * @param Direction 
 */
void ARHBasePlayer::ServerFireWeapon_Implementation(const FVector& Direction)
{
	ProjectileSpawn(Direction);
}

bool ARHBasePlayer::ServerFireWeapon_Validate(const FVector& Direction)
{
	return true;
}

/**
 * Spawns projectile in the server 
 * @param Direction 
 */
void ARHBasePlayer::ProjectileSpawn(const FVector& Direction) const
{
	// Projectile spawn loc, can be weapon bone in the future
	FVector MuzzleLocation = GetActorLocation(); //+ FTransform(GetControlRotation()).TransformVector(MuzzleOffset);
	FRotator MuzzleRotation = Direction.Rotation();
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		// Spawn the projectile at the muzzle.
		if (ARHProjectile* Projectile = World->SpawnActor<ARHProjectile>(
			ProjectileClass, MuzzleLocation, MuzzleRotation))
		{
			// Disable collision with the player immediately on spawn
			Projectile->SetActorEnableCollision(false);
			FTimerHandle TimerHandle; // Set a timer to re-enable collision after a very short delay
			World->GetTimerManager().SetTimer(TimerHandle, [Projectile]()
			{
				if (Projectile)
				{
					Projectile->SetActorEnableCollision(true);
				}
			}, 0.1f, false);


			// Set the projectile's direction to the fire direction
			Projectile->SetActorRotation(MuzzleRotation);
			Projectile->SetLifeSpan(5.0f);

			// Draw a debug line for visualization
			DrawDebugLine(GetWorld(), MuzzleLocation, MuzzleLocation + Direction * 1000, FColor::Green, true, 20.0f, 0,
			              5.0f);
			DrawDebugSphere(GetWorld(), MuzzleLocation + Direction * 1000, 32.0f, 32, FColor::Red, true, 20.0f);
		}
	}
}

void ARHBasePlayer::FireWeapon(const FVector& Direction)
{
	//todo: check if already too many projectiles exist or control fire rate here, and a validation in the server can be added too later.

	UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
	ServerFireWeapon(Direction);
}

void ARHBasePlayer::Tap(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	float MouseLocationX, MouseLocationY;
	if (PlayerController && PlayerController->GetMousePosition(MouseLocationX, MouseLocationY))
	{
		FVector2D MousePosition = FVector2D(MouseLocationX, MouseLocationY);

		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation); // Get the camera position and rotation

		FVector WorldDirection;
		PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, CameraLocation,
		                                                 WorldDirection); // Calculate world direction

		FVector EndPoint = CameraLocation + (WorldDirection * 10000); // Adjust length as needed

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this); // Ignore the player

		// Perform the raycast
		if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndPoint, ECC_Visibility, QueryParams))
		{
			// We hit something
			FVector HitLocation = HitResult.Location;
			FVector FireDirection = (HitLocation - GetActorLocation()).GetSafeNormal();

			// Optional: Draw debug line from camera to hit location
			if (GEngine)
			{
				DrawDebugSphere(GetWorld(), HitLocation, 16.0f, 4, FColor::Blue, false, 20.0f);
			}

			DrawDebugLine(GetWorld(), GetActorLocation(), HitResult.Location, FColor::Red, true, -1.0f, 0, 5.0f);
			// Now that we have the direction, fire the weapon in that direction
			FireWeapon(FireDirection);
		}
	}
}

#pragma endregion

#pragma region damage/health


/**
 * Apply damage to this actor.
 * Unreal's default function, currently overriden. 
 * @param DamageAmount 
 * @param DamageEvent 
 * @param EventInstigator 
 * @param DamageCauser 
 * @return 
 */
float ARHBasePlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("ReceiveDamage called with DamageAmount: %f"), ActualDamage);
		UE_LOG(LogTemp, Warning, TEXT("Player health now: %f"), Health);
		Health -= ActualDamage;
		if (Health <= 0)
		{
			// Handle death
			HandleDeath();
		}

		OnRep_Health(); // Manually call to update locally, replication handles remote updates
	}


	return ActualDamage;
}

#pragma endregion

#pragma region Input

void ARHBasePlayer::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
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

// Called to bind functionality to input
void ARHBasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (GetWorld()->IsNetMode(NM_Client))
	{
		/* new input system */
		APlayerController* PC = Cast<APlayerController>(GetController());

		//Get the local player subsystem
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PC->GetLocalPlayer());
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputMapping, 0);

		// Get the EnhancedInputComponent
		UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

		//the check macro for* conditions that should never fail during runtime.
		check(PEI && "PlayerInputComponent is not initialized");
		check(InputActions && "PlayerInputActions is not initialized");

		// Bind the actions
		PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ARHBasePlayer::Move);
		PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ARHBasePlayer::Look);
		PEI->BindAction(InputActions->InputTap, ETriggerEvent::Triggered, this, &ARHBasePlayer::Tap);
	}
}

#pragma endregion

#pragma region replication
void ARHBasePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARHBasePlayer, Health);
	DOREPLIFETIME(ARHBasePlayer, PlayerName);
	DOREPLIFETIME(ARHBasePlayer, PlayerColor);
	// Add oth
}

void ARHBasePlayer::OnRep_PlayerName()
{
	// Update the widget with the new player name
	UUserWidget* NameTagWidget = NameTagComponent->GetUserWidgetObject();
	if (NameTagWidget)
	{
		UTextBlock* NameText = Cast<UTextBlock>(NameTagWidget->GetWidgetFromName(TEXT("NameText")));
		if (NameText)
		{
			NameText->SetText(FText::FromString(PlayerName));
		}
	}
}

void ARHBasePlayer::OnRep_PlayerColor()
{
	if (PlayerMesh && PlayerMaterialInstance)
	{
		// Create a dynamic material instance from the assigned material instance
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
			PlayerMaterialInstance, this);

		if (DynamicMaterialInstance)
		{
			// Apply the dynamic material instance to the player mesh
			PlayerMesh->SetMaterial(0, DynamicMaterialInstance);

			// Update the color on the dynamic material instance
			DynamicMaterialInstance->SetVectorParameterValue(FName("BaseColor"), PlayerColor);
		}
	}
}

void ARHBasePlayer::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health);
}

#pragma endregion

bool ARHBasePlayer::AddResource(EResourceType ResourceType, int32 Amount)
{
	if(ResourceInventory.Num() + 4 > MaxInventorySize)
		return false;
	
	for (int i = 0; i < 4; ++i)
	{
		ResourceInventory.Add(ResourceType);
	}
	return true; 
}

//server only
int32 ARHBasePlayer::DropResources()
{
	const int32 TotalResources = ResourceInventory.Num(); // Get the count of all resources
	ResourceInventory.Empty(); // Clear the inventory
	return TotalResources; // Return the total count of resources dropped
}

void ARHBasePlayer::HandleResourcePickup(const FString& ResourceType, int32 Amount)
{
	// if (!AddResource(ResourceType, Amount))
	{
		// Handle what happens if resource can't be added because inventory is full
		// For example, display a message or ignore the pickup
	}
}

void ARHBasePlayer::HandleDeath()
{
	// Deactivate player controls and possibly hide the player model or trigger a death animation
	GetController()->DisableInput(nullptr);

	// Optionally make the player invisible or play a death animation
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// Log death
	UE_LOG(LogTemp, Warning, TEXT("Player has died. Scheduling respawn."));
	OnPlayerDead.Broadcast();

	// Schedule respawn
	FTimerHandle RespawnTimerHandle;
	float RespawnDelay = 20.0f; // Delay in seconds before respawning the player
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,
	                                       FTimerDelegate::CreateUObject(this, &ARHBasePlayer::Respawn), RespawnDelay,
	                                       false);
}

void ARHBasePlayer::Respawn()
{
	// Ensure the controller is valid
	AController* MyController = GetController();
	if (MyController && MyController->IsPlayerController())
	{
		// Call the game mode to respawn the player
		ARHGameModeBase* GM = Cast<ARHGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->SpawnPlayerAtTeamStart(MyController);
		}
	}

	// Reactivate player controls
	MyController->EnableInput(nullptr);
	OnPlayerRespawn.Broadcast();

	// Make the player visible again
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

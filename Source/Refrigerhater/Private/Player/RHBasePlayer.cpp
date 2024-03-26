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
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Input/RHInputConfigData.h"
#include "Weapon/THProjectile.h"

#pragma region init

// List of potential player names
static const TArray<FString> PossiblePlayerNames = {
	TEXT("Maverick"),
	TEXT("Prajwal"),
	TEXT("Asheen"),
	TEXT("Soorya"),
	TEXT("Prince"),
	TEXT("Phoenix")
};

// Sets default values
ARHBasePlayer::ARHBasePlayer()
{
	bReplicates = true;

	Health = 100.f;
	ResourceCapacity = 10; 

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
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
	
	AController* MyController = GetController();
	if (MyController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is still null in BeginPlay"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Got controller: %s"), *MyController->GetName());
	}

	if(HasAuthority())
	{
		// Randomize player color using HSV
		const uint8 Hue = FMath::RandRange(0, 255);
		constexpr uint8 Saturation = 255;
		const uint8 Value = FMath::RandRange(160, 255); // Range from 180 to 255 to ensure brightness

		PlayerColor = FLinearColor::MakeFromHSV8(Hue, Saturation, Value);

		// Delay the initialization of the name tag to ensure all components are ready
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARHBasePlayer::InitializeNameTag, 4.0f, false);
	}
}

void ARHBasePlayer::InitializeNameTag()
{
	FString RandomPlayerName = PossiblePlayerNames[FMath::RandRange(0, PossiblePlayerNames.Num() - 1)] + " ";
	RandomPlayerName.AppendInt(FMath::RandRange(10, 99));
	PlayerName = RandomPlayerName;
}

void ARHBasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion

void ARHBasePlayer::ServerFireWeapon_Implementation(const FVector& Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("Server firingggggg"));
	MulticastFireWeapon(Direction);
}

bool ARHBasePlayer::ServerFireWeapon_Validate(const FVector& Direction)
{
	return true; // Add validation logic if needed
}

void ARHBasePlayer::MulticastFireWeapon_Implementation(const FVector& Direction)
{
	if (ProjectileClass != nullptr)
	{
		// Check if this is the owning client and skip the following logic if it is
		if (IsLocallyControlled())
		{
			return;
		}
		
		ProjectileSpawn(Direction);
	}
}

void ARHBasePlayer::ProjectileSpawn(const FVector& Direction)
{
	// The muzzle location is typically where you want the projectile to spawn
	FVector MuzzleLocation = GetActorLocation() + FTransform(GetControlRotation()).TransformVector(MuzzleOffset);
        
	// Instead of using GetControlRotation, use the direction to create a rotation
	// The direction vector should point outwards from the front of the projectile
	FRotator MuzzleRotation = Direction.Rotation();
        
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		// Spawn the projectile at the muzzle.
		ATHProjectile* Projectile = World->SpawnActor<ATHProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation);
		if (Projectile)
		{
			// Set the projectile's direction to the fire direction
			Projectile->SetActorRotation(MuzzleRotation);
                
			// // If the projectile has a Projectile Movement component, you may also need to directly set the velocity
			// if (Projectile->ProjectileMovement)
			// {
			// 	Projectile->ProjectileMovement->Velocity = FireDirection * Projectile->ProjectileMovement->InitialSpeed;
			// }
		}
	}
}

void ARHBasePlayer::ReceiveDamage(float DamageAmount)
{
	if (HasAuthority())
	{
		Health -= DamageAmount;
		if (Health <= 0)
		{
			// Handle death
		}

		OnRep_Health(); // Manually call to update locally, replication handles remote updates
	}
}

void ARHBasePlayer::OnRep_Health()
{
	// React to health changes, e.g., update UI
}



void ARHBasePlayer::FireWeapon(const FVector& Direction)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("has autorityy"));
		//MulticastFireWeapon(Direction);
		ProjectileSpawn(Direction);
	}
	else
	{
		ServerFireWeapon(Direction);
	}
}

void ARHBasePlayer::GatherResources()
{
	
}


#pragma region Input

void ARHBasePlayer::Move(const FInputActionValue& InputActionValue)
{

	const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), -MoveValue.Y);
	
	// x = forward (Red), y is right (Green), z is up (Blue)
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, -MoveValue.X);
	//}
}

void ARHBasePlayer::Look(const FInputActionValue& InputActionValue)
{
	
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
		PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, CameraLocation, WorldDirection); // Calculate world direction

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
				DrawDebugSphere(GetWorld(), HitLocation, 16.0f, 4, FColor::Blue, false, 5.0f);
			}

			// Now that we have the direction, fire the weapon in that direction
			FireWeapon(FireDirection);
		}
	}
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

#pragma endregion

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
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(PlayerMaterialInstance, this);
        
		if (DynamicMaterialInstance)
		{
			// Apply the dynamic material instance to the player mesh
			PlayerMesh->SetMaterial(0, DynamicMaterialInstance);
            
			// Update the color on the dynamic material instance
			DynamicMaterialInstance->SetVectorParameterValue(FName("BaseColor"), PlayerColor);
		}
	}
}

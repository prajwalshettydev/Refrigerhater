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



// List of potential player names
static const TArray<FString> PossiblePlayerNames = {
	TEXT("Maverick"),
	TEXT("Prajwal"),
	TEXT("Asheen"),
	TEXT("Soorya"),
	TEXT("Prince"),
	TEXT("Phoenix")
};

void ARHBasePlayer::ServerFireWeapon_Implementation()
{
	MulticastFireWeapon();
}

bool ARHBasePlayer::ServerFireWeapon_Validate()
{
	return true; // Add validation logic if needed
}

void ARHBasePlayer::MulticastFireWeapon_Implementation()
{
	if (ProjectileClass != nullptr)
	{
		FVector MuzzleLocation = GetActorLocation() + FTransform(GetControlRotation()).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = GetControlRotation();

		UWorld* World = GetWorld();
		if (World != nullptr)
		{
			ATHProjectile* Projectile = World->SpawnActor<ATHProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation);
			// Additional setup for your projectile can go here
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

// Sets default values
ARHBasePlayer::ARHBasePlayer()
{
	bReplicates = true;
	// Initialize player properties
	Health = 100.f;
	ResourceCapacity = 10; // Default value

	// Setup mesh component
	//PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	RootComponent = GetCapsuleComponent();

	// Create a skeletal mesh component and set it as the primary mesh for the player character
	// PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	// PlayerMesh->SetupAttachment(RootComponent);

	// Assuming the parent class has already created a Skeletal Mesh Component named "CharacterMesh0"
	PlayerMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));

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


	// Create the widget component and attach it to the player's root component
	NameTagComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagComponent"));
	NameTagComponent->SetupAttachment(RootComponent);
	NameTagComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameTagComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Adjust as necessary
	NameTagComponent->SetDrawSize(FVector2D(200.0f, 50.0f)); // Adjust as necessary

	// Ensure that the name tag component replicates
	//NameTagComponent->SetIsReplicated(true);

}

void ARHBasePlayer::InitializeNameTag()
{
	
	// Randomize player name and color
	FString RandomPlayerName = PossiblePlayerNames[FMath::RandRange(0, PossiblePlayerNames.Num() - 1)] + " ";
	RandomPlayerName.AppendInt(FMath::RandRange(10, 99));
	PlayerName = RandomPlayerName;

	
}

// Called when the game starts or when spawned
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
		// Set default player name and color
		// PlayerName = TEXT("Player");
		// PlayerColor = FLinearColor::Red;
		UE_LOG(LogPlayer, Warning, TEXT("PlayerColor sett"));



		// // Generate a random bright color
		// FLinearColor RandomPlayerColor = FLinearColor(
		// 	FMath::FRandRange(0.7f, 1.0f), // Random float between 0.5 and 1.0 for Red
		// 	FMath::FRandRange(0.7f, 1.0f), // Random float between 0.5 and 1.0 for Green
		// 	FMath::FRandRange(0.7f, 1.0f), // Random float between 0.5 and 1.0 for Blue
		// 	1.0f                           // Alpha set to 1.0 (fully opaque)
		// );
		// PlayerColor = RandomPlayerColor;

		
		// Randomize player color using HSV
		const uint8 Hue = FMath::RandRange(0, 255);
		constexpr uint8 Saturation = 255;
		const uint8 Value = FMath::RandRange(160, 255); // Range from 180 to 255 to ensure brightness

		PlayerColor = FLinearColor::MakeFromHSV8(Hue, Saturation, Value);
		//OnRep_PlayerName();
		//OnRep_PlayerColor();

		// Delay the initialization of the name tag to ensure all components are ready
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARHBasePlayer::InitializeNameTag, 0.5f, false);
	}
}

void ARHBasePlayer::FireWeapon()
{
	if (HasAuthority())
	{
		MulticastFireWeapon();
	}
	else
	{
		ServerFireWeapon();
	}
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
	DOREPLIFETIME(ARHBasePlayer, PlayerName);
	DOREPLIFETIME(ARHBasePlayer, PlayerColor);
	// Add oth
}


void ARHBasePlayer::OnRep_PlayerName()
{
	UE_LOG(LogPlayer, Warning, TEXT("OnRep_PlayerName"));
	// Update the widget with the new player name
	UUserWidget* NameTagWidget = NameTagComponent->GetUserWidgetObject();
	if (NameTagWidget)
	{
		UE_LOG(LogPlayer, Warning, TEXT("NameTagWidget found."));
		UTextBlock* NameText = Cast<UTextBlock>(NameTagWidget->GetWidgetFromName(TEXT("NameText")));
		if (NameText)
		{
			UE_LOG(LogPlayer, Warning, TEXT("NameText found, setting text to: %s"), *PlayerName);
			NameText->SetText(FText::FromString(PlayerName));
		}
		else
		{
			UE_LOG(LogPlayer, Warning, TEXT("NameText not found."));
		}
	}
	else
	{
		UE_LOG(LogPlayer, Warning, TEXT("NameTagWidget not found."));
	}
}

void ARHBasePlayer::OnRep_PlayerColor()
{
	UE_LOG(LogPlayer, Warning, TEXT("OnRep_PlayerColor"));
	// Update the player material with the new color
	UpdatePlayerColor();
}

void ARHBasePlayer::UpdatePlayerColor()
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
            
			UE_LOG(LogPlayer, Warning, TEXT("Player color updated."));
		}
		else
		{
			UE_LOG(LogPlayer, Warning, TEXT("Failed to create a dynamic material instance."));
		}
	}
	else
	{
		UE_LOG(LogPlayer, Warning, TEXT("PlayerMesh or PlayerMaterialInstance is not set."));
	}
}
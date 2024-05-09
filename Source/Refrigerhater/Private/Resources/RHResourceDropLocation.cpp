// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHResourceDropLocation.h"

#include "RHGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Game/RHGameStateBase.h"
#include "Player/RHBasePlayer.h"


// Sets default values
ARHResourceDropLocation::ARHResourceDropLocation()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	DropZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DropZone"));
	DropZone->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = DropZone;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetText(FText::FromString(TEXT("Drop Location")));
	TextComponent->SetWorldSize(100.0f);  // Adjust size as needed
	TextComponent->SetRelativeLocation(FVector(0, 0, 100));  // Adjust to position text above the mesh
}

void ARHResourceDropLocation::BeginPlay()
{
	Super::BeginPlay();
	DropZone->OnComponentBeginOverlap.AddDynamic(this, &ARHResourceDropLocation::OnResourceDropped);
}

void ARHResourceDropLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentAngleDegrees += MovementSpeed * DeltaTime;
	if (CurrentAngleDegrees >= 360.0f)
	{
		CurrentAngleDegrees -= 360.0f;  // Wrap angle to prevent overflow
	}

	// Calculate new position
	float Rad = FMath::DegreesToRadians(CurrentAngleDegrees);
	FVector NewLocation = GetLevelCenter() + FVector(FMath::Cos(Rad) * MovementRadius, FMath::Sin(Rad) * MovementRadius, 70.0f);
	SetActorLocation(NewLocation);
}

FVector ARHResourceDropLocation::GetLevelCenter() const
{
	// Placeholder for getting the center of the level; adjust as necessary
	return FVector(0.0f, 0.0f, 0.0f);
}

void ARHResourceDropLocation::OnResourceDropped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// This function would be triggered when a pawn enters the drop zone.
	// Actual inventory checking and resource dropping logic should be handled here.

	if(HasAuthority())
	{
		if (ARHBasePlayer* Pawn = Cast<ARHBasePlayer>(OtherActor) )
		{
			UE_LOG(LogTemp, Warning, TEXT("resoussrce drop location overlap, %s "), *OtherActor->GetName());
			const int32 points = Pawn->DropResources();

			const ARHBasePlayerState* PS = Cast<ARHBasePlayerState>(Pawn->GetPlayerState());
			ARHGameStateBase* GS = Cast<ARHGameStateBase>(GetWorld()->GetGameState());
			if(PS->Team == 0)  GS->TeamAScore += points;
			else GS->TeamBScore += points;

		}
	}
}

void ARHResourceDropLocation::NotifyDropReceived(const FString& ResourceType, int32 Quantity)
{
	// Here you would contact the GameMode to update points or similar
	// This must run on the server where the GameMode exists
	if (HasAuthority())
	{
		// Cast to your custom GameMode and call a function to handle the score update
		ARHGameModeBase* GM = Cast<ARHGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			//GM->AddPointsForTeam(ResourceType, Quantity, GetTeamID());  // Implement this function in your GameMode
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHResourceBase.h"

#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/RHBasePlayer.h"

/* so each resource can spawn, get grabbed, vanish, allot.
 * 
 *
 */

// Sets default values
ARHResourceBase::ARHResourceBase()
{
    // Set this actor to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    // Create and initialize the sphere collision component
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Pickup"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARHResourceBase::OnOverlapBegin); 
    RootComponent = CollisionComponent;

    // Create and initialize the mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    // Set a default mesh, replace with your mesh asset
    MeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/PathToYourMesh.YourMesh'")).Object);

    // Create and initialize the particle system component
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(MeshComponent);
    // Set a default particle system, replace with your particle asset
    ParticleSystemComponent->SetTemplate(ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("ParticleSystem'/Game/PathToYourParticle.YourParticle'")).Object);

    // Ensure that the actor can be detected by raycasts and overlaps
    CollisionComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void ARHResourceBase::BeginPlay()
{
    Super::BeginPlay();

    MeshComponent->SetSimulatePhysics(true);
}

// Called every frame
void ARHResourceBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Function to handle the resource pickup
void ARHResourceBase::OnPickedUpBy(APawn* Pawn)
{
    // You could add logic here to give resources to the fridge, then destroy or hide this actor
    Destroy();
}

void ARHResourceBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ARHBasePlayer* Pawn = Cast<ARHBasePlayer>(OtherActor))
    {
        OnPickedUpBy(Pawn);
    }
}
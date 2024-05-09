// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources/RHResourceBase.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
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
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
    CollisionComponent->InitBoxExtent(FVector(25,25,25));
    CollisionComponent->SetCollisionProfileName(TEXT("ResourcePreset"));
    CollisionComponent->SetEnableGravity(true);
    //CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARHResourceBase::OnOverlapBegin); 
    RootComponent = CollisionComponent;

    // Create and initialize the mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

    // Create and initialize the particle system component
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(MeshComponent);
    // Set a default particle system, replace with your particle asset;

    // Ensure that the actor can be detected by raycasts and overlaps
    CollisionComponent->SetVisibility(true);
    CollisionComponent->SetSimulatePhysics(true);
    
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment(CollisionComponent);
    ArrowComponent->SetRelativeLocation(FVector(0,0,150.0));
    ArrowComponent->SetRelativeScale3D(FVector(1,1,1));
    ArrowComponent->SetRelativeRotation(FRotator(-90, 0, 0));
    ArrowComponent->ArrowSize = 2.0f;
    ArrowComponent->ArrowLength = 60.0f;
    ArrowComponent->ArrowColor = FColor::Black;
    BaseZ = 40.0f;
    bGoingUp = true;
    MovementSpeed = 45.0f; // Set a movement speed
    MovementRange = 50.0f; // Set the range of movement

    
    SetReplicates(true);
}

// Called when the game starts or when spawned
void ARHResourceBase::BeginPlay()
{
    Super::BeginPlay();
    
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ARHResourceBase::OverlapBegin);
    //
    BaseZ = ArrowComponent->GetComponentLocation().Z;
}

void ARHResourceBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    FVector Location = ArrowComponent->GetComponentLocation();
    if (bGoingUp)
    {
        Location.Z += MovementSpeed * DeltaTime;
        if (Location.Z > BaseZ + MovementRange)
            bGoingUp = false;
    }
    else
    {
        Location.Z -= MovementSpeed * DeltaTime;
        if (Location.Z < BaseZ - MovementRange)
            bGoingUp = true;
    }
    ArrowComponent->SetWorldLocation(Location);
}


void ARHResourceBase::OnPickedUpBy(APawn* Pawn)
{
    // You could add logic here to give resources to the fridge, then destroy or hide this actor
    Destroy();
}

void ARHResourceBase::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(!HasAuthority())
        return;
    
    if (ARHBasePlayer* Pawn = Cast<ARHBasePlayer>(OtherActor) )
    {
        UE_LOG(LogTemp, Warning, TEXT("resoussrce successfull overlap, %s "), *OtherActor->GetName());
        if(Pawn->AddResource(ResourceType, 1))
        {
            OnPickedUpBy(Pawn);
        }
    }
}
 
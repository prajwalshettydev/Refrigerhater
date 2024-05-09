#include "Refrigerhater/Public/Weapon/RHProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARHProjectile::ARHProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(18.0f);
	CollisionComponent->SetCollisionProfileName("ProjectilePreset"); 
	RootComponent = CollisionComponent;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ProjectileMesh->SetupAttachment(CollisionComponent);


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	
	bReplicates = true;
}

void ARHProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &ARHProjectile::HitMesh);	
}


void ARHProjectile::HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		if (APawn* HitPawn = Cast<APawn>(OtherActor) )
		{
			if(HitPawn != GetInstigator())
			{
				UGameplayStatics::ApplyDamage(HitPawn, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
				
			
				SetLifeSpan(0.1);
				HitComponent->SetNotifyRigidBodyCollision(false);
			}
		}
	}
}
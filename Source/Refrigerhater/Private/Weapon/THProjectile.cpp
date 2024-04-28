#include "Refrigerhater/Public/Weapon/THProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATHProjectile::ATHProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize components
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Set up the collision handler
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ATHProjectile::OnHit);
	ProjectileMesh->SetNotifyRigidBodyCollision(true);

	// Set the scale of the collision capsule
	ProjectileMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATHProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATHProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATHProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only deal damage if we hit a pawn
	if (OtherActor && OtherActor != this && OtherComp && OtherComp->IsSimulatingPhysics())
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			// Apply damage
			UGameplayStatics::ApplyDamage(HitActor, DamageAmount, nullptr, this, nullptr);
		}

		// Add your effects for the hit, like a sound or a particle effect

		// Draw a debug sphere at the hit location
		if (GEngine)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 32.0f, 12, FColor::Red, false, 5.0f);
		}

		// Destroy the projectile
		Destroy();
	}
}

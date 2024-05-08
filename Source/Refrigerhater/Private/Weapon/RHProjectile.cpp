#include "Refrigerhater/Public/Weapon/RHProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARHProjectile::ARHProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize components
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;
	ProjectileMesh->bApplyImpulseOnDamage = false;

	// Create a simple sphere collider
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(18.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName("ProjectilePreset"); // Bind hit event
	RootComponent = CollisionComponent;
	CollisionComponent->bApplyImpulseOnDamage = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Set the scale of the collision capsule
	ProjectileMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	
	SetReplicates(true);
}

void ARHProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &ARHProjectile::HitMesh);	
}


void ARHProjectile::HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
FVector NormalImpulse, const FHitResult& Hit)
{
	//only in server apply damage "GetLocalRole() == ROLE_Authority"
	if (GetLocalRole() == ROLE_Authority && OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		if (APawn* HitPawn = Cast<APawn>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying damage: %f"), Damage);
			UGameplayStatics::ApplyDamage(HitPawn, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}

		// Assume SpawnImpactEffects is another method you've implemented to handle visual/sound effects
		//SpawnImpactEffects(Hit.ImpactPoint);

		SetLifeSpan(0.1);
		HitComponent->SetNotifyRigidBodyCollision(false);
	}
}
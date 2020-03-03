// Fill out your copyright notice in the Description page of Project Settings.
#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerPawn.h"

#include <UObject/ConstructorHelpers.h>

namespace
{
	const float DefaultLifetime = 3.0f;
	const float DefaultInitialSpeed = 3000.0f;
	const float DefaultMaxSpeed = 3000.0f;
}

// Sets default values
AProjectile::AProjectile()
{
	// Use a sphere as a collision representation
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAll"));

	// set up a notification for when this component hits something blocking
	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		
	SphereComponent->SetNotifyRigidBodyCollision(true);

	// Players can't walk on it
	SphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereComponent->CanCharacterStepUpOn = ECB_No;

	//Create a Sphere Mesh to Visualize the Player
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));
		SphereVisual->SetWorldScale3D(FVector(0.3f));

		static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold"));

		if (Material.Succeeded())
		{
			SphereVisual->SetMaterial(0, Material.Object);
		}

		ProjectileMesh = SphereVisual;
	}
	   
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->InitialSpeed = DefaultInitialSpeed;
	ProjectileMovement->MaxSpeed = DefaultMaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = DefaultLifetime;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Check that actor exists and is of type PLayerPawn
	if ((OtherActor != NULL) &&
		(OtherActor != this) &&
		(OtherActor->IsA(APlayerPawn::StaticClass())))
	{
		APlayerPawn* HitPlayer = Cast<APlayerPawn>(OtherActor);

		if (HitPlayer != NULL)
		{
			HitPlayer->PlayerHit();
		}
	}
	//Always Destroy on hitting blocking collider otherwise will stick and stack
	Destroy();
}
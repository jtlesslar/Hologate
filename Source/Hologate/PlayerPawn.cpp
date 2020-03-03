// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerMovementComponent.h"
#include "Projectile.h"

#include <TimerManager.h>
#include <UObject/ConstructorHelpers.h>

namespace 
{
	const float InputClampMin = -1.0f;
	const float InputClampMax = 1.0f;
	const float FireRateDefault = 0.1f;
	const float RespawnRateDefault = 2.0f;

	const int32 FullHealth = 100;
	const int32 DamagePerShot = 20;

}

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a Sphere Component for Collision Detection
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
		
	//Create a Sphere Mesh to Visualize the Player
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	
		PlayerMesh = SphereVisual;
	}
	
	//Create a Cube Mesh to Visualize the Weapon
	UStaticMeshComponent* WeaponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRepresentation"));
	WeaponVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaeponVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (WaeponVisualAsset.Succeeded())
	{
		WeaponVisual->SetStaticMesh(WaeponVisualAsset.Object);
		WeaponVisual->SetRelativeLocation(FVector(60.0f, 0.0f, 30.0f));
		WeaponVisual->SetWorldScale3D(FVector(0.2f));
		
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/CustomMaterials/M_Black.M_Black"));

		if (Material.Succeeded())
		{
			WeaponVisual->SetMaterial(0, Material.Object);
		}

		WeaponMesh = WeaponVisual;
	}

	// Create an instance of our movement component, and tell it to update the root.
	OurMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	//Set Defaults
	FireRate = FireRateDefault;
	RespawnRate = RespawnRateDefault;
	bCanShoot = true;
	bAlive = true;
	Health = FullHealth;
	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//Assign Player to Controller
	UGameplayStatics::CreatePlayer((GetWorld()));
	   
	//Hold onto Starting Location and Rotation for Respawns
	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();
	
	if (Projectile == NULL)
	{
		Projectile = AProjectile::StaticClass();
	}

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (OurMovementComponent && 
		(OurMovementComponent->UpdatedComponent == RootComponent) &&
		(!MovementDirection.IsZero()))
	{
		OurMovementComponent->AddInputVector(MovementDirection);
	}

	if (!RotationDirection.IsZero())
	{
		SetActorRotation(RotationDirection.Rotation());
	}
		
	if (bAlive && bShooting && bCanShoot)
	{
		if (Projectile != NULL)
		{
			UWorld* World = GetWorld();
						
			if (World)
			{
				FVector MuzzleOffset = (GetActorForwardVector() * 90.0f) + FVector(0.0f, 0.0f, 30.0f);
				World->SpawnActor<AProjectile>(Projectile, GetActorLocation() + MuzzleOffset, GetActorRotation());
				bCanShoot = false;
				World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPawn::ShotTimerExpired, FireRate);
			}
		}
	}

}

void APlayerPawn::ShotTimerExpired()
{
	bCanShoot = true;
}

void APlayerPawn::PlayerHit()
{
	Health -= DamagePerShot;

	if (Health <= 0)
	{
		bAlive = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimerExpired, this, &APlayerPawn::Respawn, RespawnRate);
	}

}

void APlayerPawn::Respawn()
{
	Health = FullHealth;
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);	
	SetActorLocationAndRotation(StartingLocation, StartingRotation);
	bAlive = true;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerPawn::StartShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerPawn::StopShooting);

	PlayerInputComponent->BindAxis("MoveX", this, &APlayerPawn::Move_XAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayerPawn::Move_YAxis);

	PlayerInputComponent->BindAxis("RotateX", this, &APlayerPawn::Rotate_XAxis);
	PlayerInputComponent->BindAxis("RotateY", this, &APlayerPawn::Rotate_YAxis);

}

void APlayerPawn::Move_XAxis(float AxisValue)
{
	MovementDirection.X = FMath::Clamp(AxisValue, InputClampMin, InputClampMax);
}

void APlayerPawn::Move_YAxis(float AxisValue)
{
	MovementDirection.Y = FMath::Clamp(AxisValue, InputClampMin, InputClampMax);
}

void APlayerPawn::Rotate_XAxis(float AxisValue)
{	
	RotationDirection.X = FMath::Clamp(AxisValue, InputClampMin, InputClampMax);
}

void APlayerPawn::Rotate_YAxis(float AxisValue)
{
	RotationDirection.Y = FMath::Clamp(AxisValue, InputClampMin, InputClampMax);
}

void APlayerPawn::StartShooting()
{
	bShooting = true;		
}

void APlayerPawn::StopShooting()
{
	bShooting = false;
}

UPawnMovementComponent* APlayerPawn::GetMovementComponent() const
{
	return OurMovementComponent;
}
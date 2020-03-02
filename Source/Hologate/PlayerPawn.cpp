// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Projectile.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include <Components/SphereComponent.h>
#include <UObject/ConstructorHelpers.h>
#include "PlayerMovementComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a sphere that reacts to physics
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	// Create and position a mesh component so we can see where our sphere is
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		SphereVisual->SetWorldScale3D(FVector(0.8f));
		
	}

	UStaticMeshComponent* WaeponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRepresentation"));
	WaeponVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaeponVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (WaeponVisualAsset.Succeeded())
	{
		WaeponVisual->SetStaticMesh(WaeponVisualAsset.Object);
		WaeponVisual->SetRelativeLocation(FVector(90.0f, 0.0f, 40.0f));
		WaeponVisual->SetWorldScale3D(FVector(0.2f));

	}

	FireRate = 0.1f;
	bCanFire = true;
	Health = 100;

	// Create an instance of our movement component, and tell it to update the root.
	OurMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	UGameplayStatics::CreatePlayer(GetWorld());

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle movement based on our "MoveX" and "MoveY" axes
	{

		//if (OurMovementComponent && 
		//	(OurMovementComponent->UpdatedComponent == RootComponent) &&
		if	(!CurrentVelocity.IsZero())
		{
			OurMovementComponent->AddInputVector(CurrentVelocity* DeltaTime);
		}

		//if (!CurrentVelocity.IsZero())
		//{
		//	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		//	SetActorLocation(NewLocation);
		//	
		//	

		//}

		if (!RotationDirection.IsZero())
		{
			SetActorRotation(RotationDirection.Rotation());
		}
	}

	{
		if (bGrowing)
		{

			if (bCanFire)
			{

				if (Projectile != NULL)
				{
					//todo getworld var
					FVector MuzzleOffset = (GetActorForwardVector() * 90.0f) + FVector(0.0f, 0.0f, 50.0f);
					GetWorld()->SpawnActor<AProjectile>(Projectile, GetActorLocation() + MuzzleOffset, GetActorRotation());
					bCanFire = false;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPawn::ShotTimerExpired, FireRate);
				}
			}

		}
	}


}

void APlayerPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void APlayerPawn::PlayerHit()
{
	Health -= 20;

	if (Health <= 0)
	{
		//todo respawn function
		Health = 100;
		SetActorLocationAndRotation(StartingLocation,StartingRotation);

	}

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Grow", IE_Pressed, this, &APlayerPawn::StartGrowing);
	PlayerInputComponent->BindAction("Grow", IE_Released, this, &APlayerPawn::StopGrowing);

	PlayerInputComponent->BindAxis("MoveX", this, &APlayerPawn::Move_XAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayerPawn::Move_YAxis);

	PlayerInputComponent->BindAxis("RotateX", this, &APlayerPawn::Rotate_XAxis);
	PlayerInputComponent->BindAxis("RotateY", this, &APlayerPawn::Rotate_YAxis);

}

void APlayerPawn::Move_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 500.0f;
}

void APlayerPawn::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 500.0f;
}

void APlayerPawn::Rotate_XAxis(float AxisValue)
{
	
	RotationDirection.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);

}

void APlayerPawn::Rotate_YAxis(float AxisValue)
{
	
	RotationDirection.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void APlayerPawn::StartGrowing()
{
	bGrowing = true;		
}

void APlayerPawn::StopGrowing()
{
	bGrowing = false;
}

UPawnMovementComponent* APlayerPawn::GetMovementComponent() const
{
	return OurMovementComponent;
}
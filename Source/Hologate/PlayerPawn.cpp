// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Projectile.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Create Camera and Visible Mesh for Pawn

	//UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));

	//Attach Camera to Pawn and offset/rotate

	/*OurCamera->SetupAttachment(RootComponent);

	OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));

	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
*/
	PlayerMesh->SetupAttachment(RootComponent);

	FireRate = 0.1f;
	bCanFire = true;
	Health = 100;


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

	{
		if (bGrowing)
		{
			
			if (bCanFire)
			{
			
				if (Projectile != NULL)
				{
					//todo getworld var
					FVector MuzzleOffset = FVector(80.0f, 0.0f, 50.0f);
					GetWorld()->SpawnActor<AProjectile>(Projectile, GetActorLocation() + MuzzleOffset, GetActorRotation());
					bCanFire = false;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPawn::ShotTimerExpired, FireRate);
				}
			}

		}
	}

	// Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!CurrentVelocity.IsZero())
		{
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			SetActorLocation(NewLocation);
		}

		if (!RotationDirection.IsZero())
		{
			SetActorRotation(RotationDirection.Rotation());
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
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void APlayerPawn::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void APlayerPawn::Rotate_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	RotationDirection.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void APlayerPawn::Rotate_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
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


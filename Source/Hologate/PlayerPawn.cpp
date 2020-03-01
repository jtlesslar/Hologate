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

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Create Camera and Visible Mesh for Pawn

	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));

	//Attach Camera to Pawn and offset/rotate

	OurCamera->SetupAttachment(RootComponent);

	OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));

	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	PlayerMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		float CurrentScale = PlayerMesh->GetComponentScale().X;
		if (bGrowing)
		{
			// Grow to double size over the course of one second
			CurrentScale += DeltaTime;

			if (Projectile != NULL)
			{
				GetWorld()->SpawnActor<AProjectile>(Projectile, GetActorLocation(), GetActorRotation());
			}

		}
		else
		{
			// Shrink half as fast as we grow
			CurrentScale -= (DeltaTime * 0.5f);
		}
		// Make sure we never drop below our starting size, or increase past double size.
		CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
		PlayerMesh->SetWorldScale3D(FVector(CurrentScale));
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


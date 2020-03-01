// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class HOLOGATE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY(EditAnywhere)
		USceneComponent* PlayerMesh;

	UPROPERTY(EditAnywhere, Category = Projectile)
		TSubclassOf<class AProjectile> Projectile;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Rotate_XAxis(float AxisValue);
	void Rotate_YAxis(float AxisValue);
	void StartGrowing();
	void StopGrowing();

	//Input variables
	FVector CurrentVelocity;
	FVector RotationDirection;
	bool bGrowing;

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

private :

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

};

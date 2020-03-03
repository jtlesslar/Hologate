// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/SphereComponent.h>

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

	UPROPERTY(EditAnywhere)
		USceneComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Projectile)
		TSubclassOf<class AProjectile> Projectile;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
		float FireRate;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
		float RespawnRate;
	
	void PlayerHit();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
				
	void ShotTimerExpired();
	void Respawn();

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	class UPlayerMovementComponent* OurMovementComponent;
	
private :

	//Flag to control when a shot can be fired (Fire Rate)  
	bool bCanShoot;
	bool bAlive;

	//Handle for timer expiries
	FTimerHandle TimerHandle_ShotTimerExpired;
	FTimerHandle TimerHandle_RespawnTimerExpired;

	int32 Health;

	FVector StartingLocation;
	FRotator StartingRotation;

	//Input variables
	FVector MovementDirection;
	FVector RotationDirection;
	bool bShooting;
	
	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Rotate_XAxis(float AxisValue);
	void Rotate_YAxis(float AxisValue);
	void StartShooting();
	void StopShooting();
	   	 
};

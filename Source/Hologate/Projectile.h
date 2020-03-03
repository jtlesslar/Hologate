// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class HOLOGATE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditAnywhere, Category = Projectile)
		class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
		USceneComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
		
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return SphereComponent; }
	
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

private:
	// called when projectile hits something 
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};

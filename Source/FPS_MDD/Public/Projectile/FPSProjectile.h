// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;
struct FHitResult;

UCLASS()
class FPS_MDD_API AFPSProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Called every frame
	AFPSProjectile();
protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
};

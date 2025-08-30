// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/FPSProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
// Sets default values
AFPSProjectile::AFPSProjectile()
{
    
    PrimaryActorTick.bCanEverTick = false;

    // Collision
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(5.f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // simple default
    Collision->SetNotifyRigidBodyCollision(true);
    RootComponent = Collision;

    // Movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = Collision;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 3.f;

    Collision->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);
}

UFUNCTION()
void AFPSProjectile::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (OtherActor->ActorHasTag("Enemy"))
    {
        UE_LOG(LogTemp, Log, TEXT("Projectile hit Enemy: %s"), *OtherActor->GetName());

        // Destroy the enemy actor
        OtherActor->Destroy();
    }
    // Destroy the projectile on impact
    Destroy();
}

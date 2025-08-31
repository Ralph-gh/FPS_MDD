// EnemyCube.cpp

#include "Enemy/EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"

AEnemyCube::AEnemyCube()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	SetRootComponent(CubeMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeAsset.Object);
	}

	// Collision setup for hit events (use one path to avoid double damage)
	CubeMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CubeMesh->SetNotifyRigidBodyCollision(true);   // generate OnHit
	CubeMesh->SetGenerateOverlapEvents(false);     // disable overlap to avoid double damage

	// Bind delegates
	CubeMesh->OnComponentHit.AddDynamic(this, &AEnemyCube::OnHit);
	// If you later choose overlaps instead of hits, enable GenerateOverlapEvents and bind this:
	// CubeMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCube::OnOverlap);
}

void AEnemyCube::BeginPlay()
{
	Super::BeginPlay();

	// Find the base by tag once
	if (!TargetTag.IsNone())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetTag, Found);
		if (Found.Num() > 0)
		{
			TargetActor = Found[0];
		}
	}
}

void AEnemyCube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsValid(TargetActor)) return;

	const FVector dir = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const FVector step = dir * MoveSpeed * DeltaSeconds;

	FHitResult hit;
	AddActorWorldOffset(step, /*bSweep=*/true, &hit);

	if (hit.IsValidBlockingHit())
	{
		// If we hit the base (by tag), LOG and DESTROY
		if (AActor* hitActor = hit.GetActor())
		{
			if (hitActor->ActorHasTag(TargetTag))   // TargetTag default = "DefenseBase"
			{
				UE_LOG(LogTemp, Warning, TEXT("EnemyCube: Dealt %.0f damage to BASE (log only)"), DamageOnImpact);
				Destroy();
				return;
			}
		}

		// otherwise, slide along the surface
		const FVector slide = FVector::VectorPlaneProject(step, hit.Normal);
		if (!slide.IsNearlyZero())
		{
			AddActorWorldOffset(slide, true);
		}
	}

	AddActorLocalRotation(FRotator(0.f, 50.f * DeltaSeconds, 0.f));
}

void AEnemyCube::OnHit(UPrimitiveComponent* , AActor* OtherActor,
	UPrimitiveComponent* , FVector , const FHitResult& )
{
	if (!OtherActor || OtherActor == this) return;

	if (UHealthComponent* HC = OtherActor->FindComponentByClass<UHealthComponent>())
	{
		HC->ApplyDamage(DamageOnImpact);
		UE_LOG(LogTemp, Warning, TEXT("EnemyCube: Dealt %.0f damage to Base (log only)"), DamageOnImpact);
		Destroy();
	}
}

void AEnemyCube::OnOverlap(UPrimitiveComponent* , AActor* OtherActor,
	UPrimitiveComponent* , int32 , bool ,
	const FHitResult& )
{
	// Only used if you switch to overlap logic:
	if (!OtherActor || OtherActor == this) return;

	if (UHealthComponent* HC = OtherActor->FindComponentByClass<UHealthComponent>())
	{
		HC->ApplyDamage(DamageOnImpact);
		UE_LOG(LogTemp, Warning, TEXT("EnemyCube: Dealt %.0f damage to Base (log only)"), DamageOnImpact);
		Destroy();
	}
}

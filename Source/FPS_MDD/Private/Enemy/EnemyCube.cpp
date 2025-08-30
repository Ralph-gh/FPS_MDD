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

	const FVector MyLoc = GetActorLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();
	const FVector ToTarget = TargetLoc - MyLoc;
	const float Dist = ToTarget.Size();

	// If close enough, apply damage and self-destruct even if we didn't physically collide
	if (Dist <= AcceptanceRadius)
	{
		if (UHealthComponent* HC = TargetActor->FindComponentByClass<UHealthComponent>())
		{
			HC->ApplyDamage(DamageOnImpact);
		}
		Destroy();
		return;
	}

	// Move toward target with sweep + slide
	const FVector Dir = ToTarget.GetSafeNormal();
	const FVector Step = Dir * MoveSpeed * DeltaSeconds;

	FHitResult Hit;
	AddActorWorldOffset(Step, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		const FVector Slide = FVector::VectorPlaneProject(Step, Hit.Normal);
		if (!Slide.IsNearlyZero())
		{
			AddActorWorldOffset(Slide, true);
		}
	}

	// (Optional flair)
	// AddActorLocalRotation(FRotator(0.f, 50.f * DeltaSeconds, 0.f));
}

void AEnemyCube::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& /*Hit*/)
{
	if (!OtherActor || OtherActor == this) return;

	if (UHealthComponent* HC = OtherActor->FindComponentByClass<UHealthComponent>())
	{
		HC->ApplyDamage(DamageOnImpact);
		Destroy();
	}
}

void AEnemyCube::OnOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/)
{
	// Only used if you switch to overlap logic:
	if (!OtherActor || OtherActor == this) return;

	if (UHealthComponent* HC = OtherActor->FindComponentByClass<UHealthComponent>())
	{
		HC->ApplyDamage(DamageOnImpact);
		Destroy();
	}
}

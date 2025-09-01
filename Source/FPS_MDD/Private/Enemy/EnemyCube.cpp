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

	const FVector Dir = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const FVector Step = Dir * MoveSpeed * DeltaSeconds;

	FHitResult Hit;
	AddActorWorldOffset(Step, /*bSweep=*/true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		// 1) If we bumped into the BASE -> apply damage + destroy
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag(TargetTag))   // TargetTag default = "DefenseBase"
		{
			if (UHealthComponent* HC = HitActor->FindComponentByClass<UHealthComponent>())
			{
				HC->ApplyDamage(DamageOnImpact);
			}
			UE_LOG(LogTemp, Warning, TEXT("EnemyCube: Dealt %.0f damage to Base"), DamageOnImpact);
			Destroy();
			return;
		}

		// 2) Not the base? Slide along the surface to keep moving
		const FVector Slide = FVector::VectorPlaneProject(Step, Hit.Normal);
		if (!Slide.IsNearlyZero())
		{
			AddActorWorldOffset(Slide, /*bSweep=*/true);
		}
	}

	// Optional flair; comment out if jittery on contact
	 AddActorLocalRotation(FRotator(0.f, 50.f * DeltaSeconds, 0.f));
}


// *** FIXED: use the parameters properly (OtherActor), no undefined 'Hit' variable ***
void AEnemyCube::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/,
	const FHitResult& /*HitInfo*/)
{
	//for later use
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

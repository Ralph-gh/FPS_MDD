// Enemy/EnemySpawner.cpp
#include "Enemy/EnemySpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Sanity: ensure ranges make sense
	if (MaxRange < MinRange)
	{
		Swap(MinRange, MaxRange);
	}

	if (bAutoStart)
	{
		StartSpawning();
	}
}

FVector AEnemySpawner::GetBaseLocation() const
{
	return BaseActor ? BaseActor->GetActorLocation() : GetActorLocation();
}

void AEnemySpawner::StartSpawning()
{
	if (!GetWorld()) return;

	// If no class selected, don't start a bad loop
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner: EnemyClass not set."));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AEnemySpawner::SpawnOnce,
		FMath::Max(0.05f, SpawnInterval),
		true);
}

void AEnemySpawner::StopSpawning()
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::SpawnOnce()
{
	if (!GetWorld() || !EnemyClass) return;

	const FVector BaseLoc = GetBaseLocation();

	for (int32 i = 0; i < BatchCount; ++i)
	{
		FVector SpawnLoc;
		if (!SampleSpawnLocation(SpawnLoc))
		{
			continue;
		}

		// Optionally project to ground
		if (bProjectToGround)
		{
			const FVector Start = SpawnLoc + FVector(0, 0, GroundTraceUp);
			const FVector End = SpawnLoc - FVector(0, 0, GroundTraceDown);

			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemySpawnerGround), /*bTraceComplex=*/false, this);
			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);

			if (bHit)
			{
				SpawnLoc = Hit.ImpactPoint + FVector(0, 0, SpawnHeightOffset);
			}

			if (bDrawDebug)
			{
				DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 2.f, 0, 1.f);
			}
		}

		// Face toward base by default (so enemies "look" at the base)
		const FRotator SpawnRot = (BaseLoc - SpawnLoc).Rotation();

		FActorSpawnParameters P;
		P.Owner = this;
		P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLoc, SpawnRot, P);

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), SpawnLoc, 24.f, 12, Spawned ? FColor::Cyan : FColor::Red, false, 3.f);
			DrawDebugLine(GetWorld(), SpawnLoc, BaseLoc, FColor::Cyan, false, 3.f, 0, 0.5f);
		}
	}
}

bool AEnemySpawner::SampleSpawnLocation(FVector& OutLocation) const
{
	// Normalize the direction; if zero, fallback to +Z
	const FVector Dir = SpawnDirection.IsNearlyZero() ? FVector::UpVector : SpawnDirection.GetSafeNormal();

	// Sample a random direction within a cone around Dir
	const float ConeRad = FMath::DegreesToRadians(FMath::Clamp(ConeHalfAngleDegrees, 1.f, 89.f));
	const FVector RandDir = FMath::VRandCone(Dir, ConeRad);

	// Uniform annulus sampling (avoid clustering near center):
	// r = sqrt(U * (R^2 - r0^2) + r0^2)
	const float U = FMath::FRand();
	const float MinR2 = MinRange * MinRange;
	const float MaxR2 = MaxRange * MaxRange;
	const float Radius = FMath::Sqrt(U * (MaxR2 - MinR2) + MinR2);

	OutLocation = GetBaseLocation() + RandDir * Radius;

	// Guarantee "positive side" relative to base by dot-product test (belt & suspenders)
	const bool bPositiveSide = FVector::DotProduct((OutLocation - GetBaseLocation()), Dir) > 0.f;
	return bPositiveSide;
}

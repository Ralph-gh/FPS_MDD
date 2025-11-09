#include "Enemy/EnemySpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	if (MaxRange < MinRange) { Swap(MinRange, MaxRange); }
	if (bAutoStart) { StartSpawning(); }
}

FVector AEnemySpawner::GetBaseLocation() const
{
	return BaseActor ? BaseActor->GetActorLocation() : GetActorLocation();
}

void AEnemySpawner::StartSpawning()
{
	if (!GetWorld() || !EnemyClass) return;
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer, this, &AEnemySpawner::SpawnOnce,
		FMath::Max(0.05f, SpawnInterval), true);
}

void AEnemySpawner::StopSpawning()
{
	if (!GetWorld()) return;
	auto& TM = GetWorld()->GetTimerManager();
	TM.ClearTimer(SpawnTimer);
	TM.ClearTimer(ClearPollTimer); // also stop clear polling if any
}

void AEnemySpawner::SpawnOnce()
{
	if (!GetWorld() || !EnemyClass) return;

	// If we already met quota, begin clear-wait (safety)
	if (SpawnedThisCycle >= QuotaPerCycle)
	{
		BeginClearWait();
		return;
	}

	const FVector BaseLoc = GetBaseLocation();
	int32 SpawnedNow = 0;

	for (int32 i = 0; i < BatchCount; ++i)
	{
		if (SpawnedThisCycle + SpawnedNow >= QuotaPerCycle)
			break;

		FVector SpawnLoc;
		if (!SampleSpawnLocation(SpawnLoc)) continue;

		const FRotator SpawnRot = (BaseLoc - SpawnLoc).Rotation();

		FActorSpawnParameters P;
		P.Owner = this;
		P.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLoc, SpawnRot, P);

		if (Spawned)
		{
			++SpawnedNow;

			if (bDrawDebug)
			{
				DrawDebugSphere(GetWorld(), SpawnLoc, 24.f, 12, FColor::Cyan, false, 3.f);
				DrawDebugLine(GetWorld(), SpawnLoc, BaseLoc, FColor::Cyan, false, 3.f, 0, 0.5f);
			}
		}
		else if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), SpawnLoc, 24.f, 12, FColor::Red, false, 3.f);
		}
	}

	// Apply the batch increment
	SpawnedThisCycle += SpawnedNow;

	// If we hit quota, pause and wait until clear (Wave 1 logic)
	if (SpawnedThisCycle >= QuotaPerCycle)
	{
		BeginClearWait();
	}
}


bool AEnemySpawner::SampleSpawnLocation(FVector& OutLocation) const
{
	const FVector Dir = SpawnDirection.IsNearlyZero() ? FVector::UpVector : SpawnDirection.GetSafeNormal();
	const float ConeRad = FMath::DegreesToRadians(FMath::Clamp(ConeHalfAngleDegrees, 1.f, 89.f));
	const FVector RandDir = FMath::VRandCone(Dir, ConeRad);

	const float U = FMath::FRand();
	const float MinR2 = MinRange * MinRange;
	const float MaxR2 = MaxRange * MaxRange;
	const float Radius = FMath::Sqrt(U * (MaxR2 - MinR2) + MinR2);

	const FVector BaseLoc = GetBaseLocation();
	OutLocation = BaseLoc + RandDir * Radius;

	// Ensure we’re on the “positive” side of the chosen direction
	return FVector::DotProduct((OutLocation - BaseLoc), Dir) > 0.f;
}
int32 AEnemySpawner::CountAliveEnemies() const
{
	TArray<AActor*> Found;
	if (EnemyActorClassFilter)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyActorClassFilter, Found);
	}
	else
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), EnemyActorTag, Found);
	}
	return Found.Num();
}

void AEnemySpawner::BeginClearWait()
{
	// Stop spawning and begin polling until level is clear
	StopSpawning();
	ClearAccum = 0.f;

	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(
		ClearPollTimer, this, &AEnemySpawner::OnPollClearWait,
		PollIntervalSeconds, true);
}

void AEnemySpawner::OnPollClearWait()
{
	const int32 Alive = CountAliveEnemies();
	if (Alive == 0)
	{
		ClearAccum += PollIntervalSeconds;
		if (ClearAccum >= ClearHoldSeconds)
		{
			// Reset cycle and resume spawning
			GetWorld()->GetTimerManager().ClearTimer(ClearPollTimer);
			SpawnedThisCycle = 0;
			StartSpawning();
		}
	}
	else
	{
		ClearAccum = 0.f;
	}
}
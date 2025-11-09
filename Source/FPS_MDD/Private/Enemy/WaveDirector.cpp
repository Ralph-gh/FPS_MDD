#include "Enemy/WaveDirector.h"
#include "Enemy/EnemySpawner.h"         // Your spawner header
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWaveDirector::AWaveDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWaveDirector::BeginPlay()
{
	Super::BeginPlay();
	StartWaves();
}

void AWaveDirector::StartWaves()
{
	if (CurrentWave != EWave::None) return;
	if (!SpawnerClass || !NormalEnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveDirector: SpawnerClass or NormalEnemyClass not set."));
	}
	Advance(); // -> Wave1
}

void AWaveDirector::Advance()
{
	switch (CurrentWave)
	{
	case EWave::None:  CurrentWave = EWave::Wave1; EnterWave1(); break;
	case EWave::Wave1: CurrentWave = EWave::Wave2; EnterWave2(); break;
	case EWave::Wave2: CurrentWave = EWave::Wave3; EnterWave3(); break;
	default: CurrentWave = EWave::Done; break;
	}
}

FTransform AWaveDirector::MakeSpawnTransformRelative(const FVector& Offset) const
{
	const AActor* Ref = BaseActor ? BaseActor : this;
	const FVector Loc = Ref->GetActorLocation() + Ref->GetActorRotation().RotateVector(Offset);
	const FRotator Rot = (BaseActor ? BaseActor->GetActorForwardVector() : GetActorForwardVector()).Rotation();
	return FTransform(Rot, Loc);
}

int32 AWaveDirector::CountAliveEnemies() const
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

void AWaveDirector::EnterWave1()
{
	// Wave 1: no spawns—wait until world is clear
	ClearAccum = 0.f;
	GetWorld()->GetTimerManager().SetTimer(PollTimer, this, &AWaveDirector::OnPollWave1, PollIntervalSeconds, true);

	if (bDrawDebug)
	{
		DrawDebugString(GetWorld(),
			(BaseActor ? BaseActor->GetActorLocation() : GetActorLocation()) + FVector(0, 0, 150.f),
			TEXT("Wave 1: Clear all existing enemies"),
			nullptr, FColor::Yellow, 2.0f, true);
	}
}

void AWaveDirector::OnPollWave1()
{
	const int32 Alive = CountAliveEnemies();
	if (Alive == 0)
	{
		ClearAccum += PollIntervalSeconds;
		if (ClearAccum >= ClearHoldSeconds)
		{
			GetWorld()->GetTimerManager().ClearTimer(PollTimer);
			Advance();
		}
	}
	else
	{
		ClearAccum = 0.f;
	}
}

void AWaveDirector::ConfigureSpawner(AEnemySpawner* Spawner, bool bShadow) const
{
	if (!Spawner) return;
	Spawner->BaseActor = BaseActor ? BaseActor : const_cast<AWaveDirector*>(this);

	if (bShadow)
	{
		Spawner->EnemyClass = ShadowEnemyClass ? ShadowEnemyClass : NormalEnemyClass;
		Spawner->MinRange = Shadow_MinRange;
		Spawner->MaxRange = Shadow_MaxRange;
		Spawner->SpawnDirection = Shadow_SpawnDirection;
		Spawner->ConeHalfAngleDegrees = Shadow_ConeHalfAngleDeg;
		Spawner->SpawnInterval = Shadow_SpawnInterval;
		Spawner->BatchCount = Shadow_BatchCount;
		Spawner->bAutoStart = true;
		Spawner->bDrawDebug = bDrawDebug;
	}
	else
	{
		Spawner->EnemyClass = NormalEnemyClass;
		Spawner->MinRange = Normal_MinRange;
		Spawner->MaxRange = Normal_MaxRange;
		Spawner->SpawnDirection = Normal_SpawnDirection;
		Spawner->ConeHalfAngleDegrees = Normal_ConeHalfAngleDeg;
		Spawner->SpawnInterval = Normal_SpawnInterval;
		Spawner->BatchCount = Normal_BatchCount;
		Spawner->bAutoStart = true;
		Spawner->bDrawDebug = bDrawDebug;
	}
}

void AWaveDirector::EnterWave2()
{
	// Spawn NORMAL spawner
	if (!SpawnerClass) { UE_LOG(LogTemp, Warning, TEXT("WaveDirector: SpawnerClass not set.")); return; }

	const FTransform Xform = MakeSpawnTransformRelative(NormalSpawnerOffset);
	NormalSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(SpawnerClass, Xform, this);
	ConfigureSpawner(NormalSpawner, /*bShadow=*/false);
	UGameplayStatics::FinishSpawningActor(NormalSpawner, Xform);

	if (bDrawDebug)
	{
		DrawDebugString(GetWorld(), Xform.GetLocation() + FVector(0, 0, 120),
			TEXT("Wave 2: Normal Spawner"), nullptr, FColor::Cyan, 3.0f, true);
	}

	// Immediately advance if you want continuous progression,
	// or wait on some condition. Here we auto-advance.
	Advance();
}

void AWaveDirector::EnterWave3()
{
	// Spawn SHADOW spawner + keep Normal spawner alive
	if (!SpawnerClass) return;

	// If you also want a *second* Normal spawner in Wave 3, spawn again here.
	if (!NormalSpawner)
	{
		const FTransform NX = MakeSpawnTransformRelative(NormalSpawnerOffset);
		NormalSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(SpawnerClass, NX, this);
		ConfigureSpawner(NormalSpawner, /*bShadow=*/false);
		UGameplayStatics::FinishSpawningActor(NormalSpawner, NX);
	}

	const FTransform SX = MakeSpawnTransformRelative(ShadowSpawnerOffset);
	ShadowSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(SpawnerClass, SX, this);
	ConfigureSpawner(ShadowSpawner, /*bShadow=*/true);
	UGameplayStatics::FinishSpawningActor(ShadowSpawner, SX);

	if (bDrawDebug)
	{
		DrawDebugString(GetWorld(), SX.GetLocation() + FVector(0, 0, 120),
			TEXT("Wave 3: Shadow Spawner + Normal Spawner"), nullptr, FColor::Purple, 3.0f, true);
	}

	// Now you’re in Wave 3 (running spawners). You can add more waves later.
}

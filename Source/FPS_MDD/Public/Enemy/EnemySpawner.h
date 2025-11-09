#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class FPS_MDD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
public:
	AEnemySpawner();

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	AActor* BaseActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0"))
	float MinRange = 800.f;

	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0"))
	float MaxRange = 2000.f;

	// Default +Z; set to Base->GetActorForwardVector() if you meant “in front”.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnDirection = FVector::UpVector;

	// 89°  “positive side only” (almost a hemisphere)
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float ConeHalfAngleDegrees = 89.f;

	UPROPERTY(EditAnywhere, Category = "Spawning|Timing", meta = (ClampMin = "0.05"))
	float SpawnInterval = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Spawning|Timing", meta = (ClampMin = "1"))
	int32 BatchCount = 1;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, Category = "Spawning|Timing")
	bool bAutoStart = true;

	UFUNCTION(BlueprintCallable, Category = "Spawning") void StartSpawning();
	UFUNCTION(BlueprintCallable, Category = "Spawning") void StopSpawning();
	UFUNCTION(BlueprintCallable, Category = "Spawning") void SpawnOnce();

	// === Quota / clear-logic ===
	UPROPERTY(EditAnywhere, Category = "Spawning|Quota", meta = (ClampMin = "1"))
	int32 QuotaPerCycle = 20;  // stop after this many spawns, then wait until clear, then resume

	// Tag or class used to detect "enemies alive" for clear checks
	UPROPERTY(EditAnywhere, Category = "Spawning|Detection")
	FName EnemyActorTag = TEXT("Enemy");

	UPROPERTY(EditAnywhere, Category = "Spawning|Detection")
	TSubclassOf<AActor> EnemyActorClassFilter;

	// Polling cadence while waiting for clear
	UPROPERTY(EditAnywhere, Category = "Spawning|Detection", meta = (ClampMin = "0.1"))
	float PollIntervalSeconds = 0.5f;

	// Require the world to be clear for this long before resuming
	UPROPERTY(EditAnywhere, Category = "Spawning|Detection", meta = (ClampMin = "0.0"))
	float ClearHoldSeconds = 1.5f;


protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle SpawnTimer;
	bool SampleSpawnLocation(FVector& OutLocation) const;
	FVector GetBaseLocation() const;
	// NEW: while paused at quota, we poll for "clear"
	FTimerHandle ClearPollTimer;

	// Count spawned in current cycle; resets to 0 after a clear
	int32 SpawnedThisCycle = 0;

	float ClearAccum = 0.f;
	// NEW helpers
	int32 CountAliveEnemies() const;
	void BeginClearWait();
	void OnPollClearWait();
};

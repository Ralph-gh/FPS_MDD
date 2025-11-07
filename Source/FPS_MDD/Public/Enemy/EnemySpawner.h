// Enemy/EnemySpawner.h
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

	// What to spawn (set in Details). You can pick AEnemyCube or any enemy BP.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> EnemyClass;

	// If set, spawns use this actor's location as the "base".
	// If null, this spawner's own location is used.
	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	AActor* BaseActor = nullptr;

	// Min/Max range (world units) away from base
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0"))
	float MinRange = 800.f;

	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0"))
	float MaxRange = 2000.f;

	// Direction that defines the “allowed half-space/sector”.
	// Default is +Z (Up). If you want "in front of base", set to base->GetActorForwardVector().
	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnDirection = FVector::UpVector;

	// Cone half-angle around SpawnDirection. 89°  “positive side only” (almost a hemisphere).
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float ConeHalfAngleDegrees = 89.f;

	// Spawn cadence
	UPROPERTY(EditAnywhere, Category = "Spawning|Timing", meta = (ClampMin = "0.05"))
	float SpawnInterval = 2.5f;

	// How many to spawn each tick
	UPROPERTY(EditAnywhere, Category = "Spawning|Timing", meta = (ClampMin = "1"))
	int32 BatchCount = 1;

	// Optional: drop the point to the ground with a line trace
	UPROPERTY(EditAnywhere, Category = "Spawning|Grounding")
	bool bProjectToGround = true;

	// How high above the candidate point to start the ground trace
	UPROPERTY(EditAnywhere, Category = "Spawning|Grounding", meta = (EditCondition = "bProjectToGround"))
	float GroundTraceUp = 1500.f;

	// How far to trace down
	UPROPERTY(EditAnywhere, Category = "Spawning|Grounding", meta = (EditCondition = "bProjectToGround"))
	float GroundTraceDown = 5000.f;

	// Small lift to avoid z-fighting with the floor
	UPROPERTY(EditAnywhere, Category = "Spawning|Grounding", meta = (EditCondition = "bProjectToGround"))
	float SpawnHeightOffset = 10.f;

	// Visualize sampled locations
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = true;

	// Auto start on BeginPlay
	UPROPERTY(EditAnywhere, Category = "Spawning|Timing")
	bool bAutoStart = true;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StopSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnOnce();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle SpawnTimer;

	// Uniformly sample a point in an annulus sector pointing along SpawnDirection
	bool SampleSpawnLocation(FVector& OutLocation) const;

	FVector GetBaseLocation() const;
};

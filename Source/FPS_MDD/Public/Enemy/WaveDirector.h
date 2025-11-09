#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveDirector.generated.h"

class AEnemySpawner;
class AActor;

UCLASS()
class FPS_MDD_API AWaveDirector : public AActor
{
	GENERATED_BODY()

public:
	AWaveDirector();

	/** Base the enemies will target; if null, uses this actor’s location/forward */
	UPROPERTY(EditInstanceOnly, Category = "Wave|Refs")
	AActor* BaseActor = nullptr;

	/** Spawner class to use for Normal/Shadow. Can be the SAME class (AEnemySpawner). */
	UPROPERTY(EditAnywhere, Category = "Wave|Classes")
	TSubclassOf<AEnemySpawner> SpawnerClass;

	/** Enemy classes spawned by each spawner */
	UPROPERTY(EditAnywhere, Category = "Wave|Classes")
	TSubclassOf<AActor> NormalEnemyClass;

	UPROPERTY(EditAnywhere, Category = "Wave|Classes")
	TSubclassOf<AActor> ShadowEnemyClass;

	/** Where to place the spawners, relative to Base (or to this actor if Base is null) */
	UPROPERTY(EditAnywhere, Category = "Wave|Placement")
	FVector NormalSpawnerOffset = FVector(0, -800.f, 600.f);

	UPROPERTY(EditAnywhere, Category = "Wave|Placement")
	FVector ShadowSpawnerOffset = FVector(0, 800.f, 600.f);

	/** Normal spawner config */
	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Normal_MinRange = 800.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Normal_MaxRange = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	FVector Normal_SpawnDirection = FVector::UpVector; // +Z

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float Normal_ConeHalfAngleDeg = 60.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Normal_SpawnInterval = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	int32 Normal_BatchCount = 1;

	/** Shadow spawner config (tweak as you like) */
	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Shadow_MinRange = 900.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Shadow_MaxRange = 2200.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	FVector Shadow_SpawnDirection = FVector::UpVector; // or opposite/behind if you want

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float Shadow_ConeHalfAngleDeg = 75.f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	float Shadow_SpawnInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Wave|SpawnerConfig")
	int32 Shadow_BatchCount = 2;

	/** Tag or class filter used to detect “enemies alive” for Wave 1 cleanup gate */
	UPROPERTY(EditAnywhere, Category = "Wave|Detection")
	FName EnemyActorTag = TEXT("Enemy");

	/** Optional: also look for this class (set to your enemy base class if you prefer). Leave null to ignore. */
	UPROPERTY(EditAnywhere, Category = "Wave|Detection")
	TSubclassOf<AActor> EnemyActorClassFilter;

	/** How often to poll for "no enemies left" during Wave 1 */
	UPROPERTY(EditAnywhere, Category = "Wave|Detection", meta = (ClampMin = "0.1"))
	float PollIntervalSeconds = 0.5f;

	/** Require the world to be clear for this long before advancing from Wave 1 (debounce) */
	UPROPERTY(EditAnywhere, Category = "Wave|Detection", meta = (ClampMin = "0.0"))
	float ClearHoldSeconds = 1.5f;

	/** Debug draw */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = true;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWaves();

protected:
	virtual void BeginPlay() override;

private:
	enum class EWave : uint8 { None = 0, Wave1 = 1, Wave2 = 2, Wave3 = 3, Done = 4 };
	EWave CurrentWave = EWave::None;

	FTimerHandle PollTimer;
	float ClearAccum = 0.f;

	UPROPERTY() AEnemySpawner* NormalSpawner = nullptr;
	UPROPERTY() AEnemySpawner* ShadowSpawner = nullptr;

	FTransform MakeSpawnTransformRelative(const FVector& Offset) const;
	int32 CountAliveEnemies() const;
	void Advance();
	void EnterWave1();
	void EnterWave2();
	void EnterWave3();
	void OnPollWave1();
	void ConfigureSpawner(class AEnemySpawner* Spawner, bool bShadow) const;
};

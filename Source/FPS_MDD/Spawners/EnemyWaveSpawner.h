
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWaveSpawner.generated.h"
class UBoxComponent;

UCLASS()
class FPS_MDD_API AEnemyWaveSpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemyWaveSpawner();

    // Call from GameMode when a new wave starts (e.g., after the previous wave clears)
    UFUNCTION(BlueprintCallable, Category="Spawning")
    void SpawnWave(int32 WaveIndex);

protected:
    UPROPERTY(VisibleAnywhere, Category="Components")
    UBoxComponent* SpawnArea;

    // Set these to your Blueprints: BP_EnemyCube and BP_EnemyCubeShadow
    UPROPERTY(EditAnywhere, Category="Spawning")
    TSubclassOf<AActor> EnemyNormalClass;

    UPROPERTY(EditAnywhere, Category="Spawning")
    TSubclassOf<AActor> EnemyShadowClass;

    // Counts for Wave 2 (the first auto-spawned wave)
    UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0"))
    int32 NumNormalPerWave = 5;

    UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0"))
    int32 NumShadowPerWave = 5;

    // Optional: scale per wave (e.g., +2 each wave)
    UPROPERTY(EditAnywhere, Category="Spawning")
    int32 NormalIncrementPerWave = 0;

    UPROPERTY(EditAnywhere, Category="Spawning")
    int32 ShadowIncrementPerWave = 0;

private:
    FVector GetRandomPointInSpawnArea() const;
    void SpawnOne(TSubclassOf<AActor> ClassToSpawn);
};


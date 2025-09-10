

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Components/DimensionComponent.h"
#include "HUD/DefenseHUD.h"
#include "AFPSProjectGameModeBase.generated.h"


class UDimensionComponent; // from our component
enum class EDefenseDimension : uint8; // forward declare enum (included via the .cpp)

UCLASS()
class FPS_MDD_API AFPSProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
    AFPSProjectGameModeBase();


    // ---- Wave / Enemy tracking ----
    UFUNCTION(BlueprintPure, Category = "Waves")
    int32 GetCurrentWave() const { return CurrentWave; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    FName EnemyTag = TEXT("enemy");            // set this on your enemy actors

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves", meta = (ClampMin = "0.05"))
    float WaveCheckPeriod = 0.5f;              // seconds between checks


    // --- Score system ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    int32 Score = 0;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const { return Score; }
   

    // ----- Dimension management -----
    UFUNCTION(BlueprintCallable, Category = "Dimension")
    void ToggleDimension();

    UFUNCTION(BlueprintCallable, Category = "Dimension")
    void SetCurrentDimension(EDefenseDimension NewDim);

    UFUNCTION(BlueprintPure, Category = "Dimension")
    EDefenseDimension GetCurrentDimension() const { return CurrentDimension; }

    // Registration API (called by UDimensionComponent)
    void RegisterDimensionComponent(UDimensionComponent* Comp);
    void UnregisterDimensionComponent(UDimensionComponent* Comp);

   //score
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
protected:
	virtual void StartPlay() override;


private:
    // State
    int32 CurrentWave = 1;
    int32 LastKnownEnemyCount = 0;

    FTimerHandle WaveCheckTimer;

    // Core loop
    void StartWaveMonitor();
    void CheckForWaveClear();
    int32 CountAliveEnemies() const;
    void AdvanceWave();
    
    UPROPERTY(VisibleAnywhere, Category = "Dimension")
    EDefenseDimension CurrentDimension = EDefenseDimension::Normal;

    // Keep weak refs so dead actors don’t persist
    TSet<TWeakObjectPtr<UDimensionComponent>> RegisteredComponents;

    void BroadcastDimension();
	
};

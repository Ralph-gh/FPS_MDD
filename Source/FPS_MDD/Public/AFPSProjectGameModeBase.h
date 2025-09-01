// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Components/DimensionComponent.h"
#include "AFPSProjectGameModeBase.generated.h"

class UDimensionComponent; // from our component
enum class EDefenseDimension : uint8; // forward declare enum (included via the .cpp)

UCLASS()
class FPS_MDD_API AFPSProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
    AFPSProjectGameModeBase();

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

protected:
	virtual void StartPlay() override;


private:
    UPROPERTY(VisibleAnywhere, Category = "Dimension")
    EDefenseDimension CurrentDimension = EDefenseDimension::Normal;

    // Keep weak refs so dead actors don’t persist
    TSet<TWeakObjectPtr<UDimensionComponent>> RegisteredComponents;

    void BroadcastDimension();
	
};

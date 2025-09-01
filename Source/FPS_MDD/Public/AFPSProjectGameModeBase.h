// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AFPSProjectGameModeBase.generated.h"

/**
 * 
 */
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
protected:
	virtual void StartPlay() override;
	
};

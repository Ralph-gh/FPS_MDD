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
protected:
	virtual void StartPlay() override;
};

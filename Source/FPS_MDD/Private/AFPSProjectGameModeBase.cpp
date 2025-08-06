// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPSProjectGameModeBase.h"
#include "Engine/Engine.h"

void AFPSProjectGameModeBase::StartPlay()
{
    Super::StartPlay();

    check(GEngine != nullptr);

    // 1. To print to screen
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameModeBase! RG message"));
}
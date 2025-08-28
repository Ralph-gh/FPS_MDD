// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPSProjectGameModeBase.h"
#include "HUD/DefenseHUD.h"  
#include "Engine/Engine.h"

void AFPSProjectGameModeBase::StartPlay()
{
    Super::StartPlay();

    check(GEngine != nullptr);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
       /* // Log current state                                                              //Logs to check HUD status
        UE_LOG(LogTemp, Warning, TEXT("GM HUDClass = %s"), *GetNameSafe(HUDClass));
       // UE_LOG(LogTemp, Warning, TEXT("PC bShowHUD = %d"), PC->bShowHUD ? 1 : 0);
        UE_LOG(LogTemp, Warning, TEXT("Existing HUD = %s"), *GetNameSafe(PC->GetHUD()));

        // Force the HUD (even if something didn’t wire up)
        PC->ClientSetHUD(ADefenseHUD::StaticClass());

        UE_LOG(LogTemp, Warning, TEXT("After ClientSetHUD -> HUD = %s"), *GetNameSafe(PC->GetHUD()));
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("ClientSetHUD called"));*/


        if (ADefenseHUD* HUD = PC->GetHUD<ADefenseHUD>())
        {
            HUD->SetWave(1);   // shows "Wave 1"
        }
    }

    // 1. To print to screen
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameModeBase! RG message"));
  
   // int TestNumber = 10;
   // bool BoolTest = false;
    //FString NameTest = ("Your string here");

}

AFPSProjectGameModeBase::AFPSProjectGameModeBase()
{
    // This is the constructor
    HUDClass = ADefenseHUD::StaticClass();
    // Runs when the object is created (before BeginPlay/StartPlay)
}
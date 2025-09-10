// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPSProjectGameModeBase.h"
#include "Components/DimensionComponent.h"
#include "HUD/DefenseHUD.h"  
#include "Engine/Engine.h"
#include <Kismet/GameplayStatics.h>

void AFPSProjectGameModeBase::StartPlay()
{
    Super::StartPlay();
    BroadcastDimension(); // ensure initial state applied

    check(GEngine != nullptr);

    // Initialize HUD with wave 1 (if your HUD exposes SetWave)
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (ADefenseHUD* HUD = PC ? Cast<ADefenseHUD>(PC->GetHUD()) : nullptr)
        {
            HUD->SetWave(CurrentWave);
        }
    }

    // Start background monitoring
    StartWaveMonitor();


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

void AFPSProjectGameModeBase::AddScore(int32 Amount)
{
    Score += Amount;

    UE_LOG(LogTemp, Log, TEXT("Score updated: %d"), Score);

    // Optional: tell HUD to update immediately
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (ADefenseHUD* HUD = PC ? Cast<ADefenseHUD>(PC->GetHUD()) : nullptr)
        {
            HUD->SetScore(Score);
        }
    }
}

void AFPSProjectGameModeBase::ToggleDimension()
{
    SetCurrentDimension(CurrentDimension == EDefenseDimension::Normal
        ? EDefenseDimension::Shadow
        : EDefenseDimension::Normal);
}

void AFPSProjectGameModeBase::SetCurrentDimension(EDefenseDimension NewDim)
{
    if (CurrentDimension == NewDim) return;
    CurrentDimension = NewDim;
    BroadcastDimension();

    UE_LOG(LogTemp, Log, TEXT("Dimension switched to: %s"),
        (NewDim == EDefenseDimension::Normal ? TEXT("Normal") : TEXT("Shadow")));
}

void AFPSProjectGameModeBase::RegisterDimensionComponent(UDimensionComponent* Comp)
{
    if (!Comp) return;
    RegisteredComponents.Add(Comp);
    Comp->ApplyForDimension(CurrentDimension);
}

void AFPSProjectGameModeBase::UnregisterDimensionComponent(UDimensionComponent* Comp)
{
    if (!Comp) return;
    RegisteredComponents.Remove(Comp);
}

//initialize game score
void AFPSProjectGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    const FString ScoreStr = UGameplayStatics::ParseOption(Options, TEXT("FinalScore"));
    if (!ScoreStr.IsEmpty()) { Score = FCString::Atoi(*ScoreStr); }
}

void AFPSProjectGameModeBase::BroadcastDimension()
{
    for (auto It = RegisteredComponents.CreateIterator(); It; ++It)
    {
        if (UDimensionComponent* Comp = It->Get())
        {
            Comp->ApplyForDimension(CurrentDimension);
        }
        else
        {
            It.RemoveCurrent();
        }
    }
}

void AFPSProjectGameModeBase::StartWaveMonitor() // monitors enemies and check fo clear
{
    LastKnownEnemyCount = CountAliveEnemies();
    GetWorldTimerManager().SetTimer(
        WaveCheckTimer,
        this,
        &AFPSProjectGameModeBase::CheckForWaveClear,
        WaveCheckPeriod,
        true
    );
}

void AFPSProjectGameModeBase::CheckForWaveClear()
{
    const int32 Count = CountAliveEnemies();

    // Only advance when we had some enemies and now have none
    if (Count == 0 && LastKnownEnemyCount > 0)
    {
        AdvanceWave();
    }

    LastKnownEnemyCount = Count;
}

int32 AFPSProjectGameModeBase::CountAliveEnemies() const
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), EnemyTag, Found);

    int32 Alive = 0;
    for (AActor* A : Found)
    {
        if (IsValid(A))      // covers null & pending-kill
        {
            ++Alive;
        }
    }
    return Alive;
}


void AFPSProjectGameModeBase::AdvanceWave()
{
    ++CurrentWave;

    UE_LOG(LogTemp, Log, TEXT("Wave cleared! Advancing to Wave %d"), CurrentWave);

    // Update HUD immediately
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (ADefenseHUD* HUD = PC ? Cast<ADefenseHUD>(PC->GetHUD()) : nullptr)
        {
            HUD->SetWave(CurrentWave);
        }
    }

    // TODO: Spawn next wave (hook your spawner here)
    // Example patterns (implement later):
    // 1) Find a placed spawner in level and call SpawnWave(CurrentWave)
    //    TArray<AActor*> Spawners; UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), Spawners);
    //    if (Spawners.Num() > 0) { Cast<AEnemySpawner>(Spawners[0])->SpawnWave(CurrentWave); }
    //
    // 2) Or just schedule a delayed spawn via GetWorldTimerManager().SetTimer(...)
}
#include "Components/HealthComponent.h"
#include "HUD/DefenseHUD.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include <GUI/Slate/SSettingsWidget.h>

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	UpdateHUD(); // push initial value at game start
}

void UHealthComponent::SetMaxHealth(float NewMax)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	UpdateHUD();
}

void UHealthComponent::ApplyDamage(float Amount)
{
    if (Amount <= 0.f || IsDead()) return;
    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);
    UpdateHUD();

    if (IsDead())
    {
        if (AActor* Owner = GetOwner())
        {
            UE_LOG(LogTemp, Warning, TEXT("%s died"), *Owner->GetName());
        }
    }
}

void UHealthComponent::Heal(float Amount)
{
    if (Amount <= 0.f || IsDead()) return;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
    UpdateHUD();
}

void UHealthComponent::UpdateHUD() const
{

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (ADefenseHUD* HUD = PC ? Cast<ADefenseHUD>(PC->GetHUD()) : nullptr)
            {
                HUD->SetBaseHealth(CurrentHealth, MaxHealth);
            }
        }
    }
}

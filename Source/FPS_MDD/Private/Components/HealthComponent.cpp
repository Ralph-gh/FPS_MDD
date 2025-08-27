// HealthComponent.cpp
#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
}

void UHealthComponent::SetMaxHealth(float NewMax)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
}

void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.f || CurrentHealth <= 0.f) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
}

void UHealthComponent::Damage(float Amount)
{
	if (Amount <= 0.f || CurrentHealth <= 0.f) return;
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);
}

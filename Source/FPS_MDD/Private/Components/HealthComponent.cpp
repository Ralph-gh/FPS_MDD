#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
	if (Amount <= 0.f) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
}

void UHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.f) return;
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);

	if (IsDead())
	{
		if (AActor* Owner = GetOwner())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s has died"), *Owner->GetName());
		}
	}
}

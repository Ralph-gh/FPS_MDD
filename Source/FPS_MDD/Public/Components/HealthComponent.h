// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_MDD_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// --- Data ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.f;

	// --- Helpers ---
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const
	{
		return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;
	}

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMax);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Damage(float Amount); // subtract health (positive Amount reduces)

protected:
	virtual void BeginPlay() override;
};

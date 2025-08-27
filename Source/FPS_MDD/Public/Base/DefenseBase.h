// DefenseBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefenseBase.generated.h"

class UStaticMeshComponent;
class UHealthComponent;

UCLASS()
class FPS_MDD_API ADefenseBase : public AActor
{
	GENERATED_BODY()

public:
	ADefenseBase();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* Health;

	// HUD sync
	void UpdateHUD() const;

public:
	// Simple Blueprint-callable wrappers so you can test from BP
	UFUNCTION(BlueprintCallable, Category = "Base|Health")
	void BaseSetMaxHealth(float NewMax);

	UFUNCTION(BlueprintCallable, Category = "Base|Health")
	void BaseHeal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Base|Health")
	void BaseDamage(float Amount);
};

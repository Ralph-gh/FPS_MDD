// EnemyCube.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCube.generated.h"

class UStaticMeshComponent;

UCLASS()
class FPS_MDD_API AEnemyCube : public AActor
{
	GENERATED_BODY()

public:
	AEnemyCube();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

private:
	// Visual / collision
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* CubeMesh = nullptr;

	// Movement
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
	float MoveSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 50.f;

	// Target discovery
	UPROPERTY(EditAnywhere, Category = "Target")
	FName TargetTag = TEXT("DefenseBase");

	UPROPERTY(Transient)
	AActor* TargetActor = nullptr;

	// Damage on contact
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.0"))
	float DamageOnImpact = 20.f; //Change damage here
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	// Keep only if you want overlaps (we’ll disable generation in cpp to avoid double damage)
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
